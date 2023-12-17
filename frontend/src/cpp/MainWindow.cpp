#include <cmath>
#include <memory.h>
#include <sstream>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include <raylib.h>
#include <raymath.h>
#pragma GCC diagnostic pop
#include <raygui/raygui.h>
#include <rpng/rpng.h>

#include "contomap/editor/Selections.h"
#include "contomap/editor/Styles.h"
#include "contomap/frontend/Colors.h"
#include "contomap/frontend/DirectMapRenderer.h"
#include "contomap/frontend/FocusInterceptor.h"
#include "contomap/frontend/HelpDialog.h"
#include "contomap/frontend/LoadDialog.h"
#include "contomap/frontend/LocateTopicAndActDialog.h"
#include "contomap/frontend/MainWindow.h"
#include "contomap/frontend/MapRenderList.h"
#include "contomap/frontend/MapRenderMeasurer.h"
#include "contomap/frontend/Names.h"
#include "contomap/frontend/RenameTopicDialog.h"
#include "contomap/frontend/SaveAsDialog.h"
#include "contomap/frontend/StyleDialog.h"
#include "contomap/infrastructure/serial/BinaryDecoder.h"
#include "contomap/infrastructure/serial/BinaryEncoder.h"
#include "contomap/model/Associations.h"
#include "contomap/model/Topics.h"

using contomap::editor::InputRequestHandler;
using contomap::editor::SelectedType;
using contomap::editor::SelectionAction;
using contomap::editor::Selections;
using contomap::editor::Styles;
using contomap::frontend::Colors;
using contomap::frontend::DirectMapRenderer;
using contomap::frontend::FocusInterceptor;
using contomap::frontend::LocateTopicAndActDialog;
using contomap::frontend::MainWindow;
using contomap::frontend::MapCamera;
using contomap::frontend::MapRenderer;
using contomap::frontend::Names;
using contomap::frontend::RenameTopicDialog;
using contomap::frontend::RenderContext;
using contomap::model::Association;
using contomap::model::Associations;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::Role;
using contomap::model::SpacialCoordinate;
using contomap::model::Style;
using contomap::model::Topic;
using contomap::model::TopicName;
using contomap::model::TopicNameValue;
using contomap::model::Topics;

MainWindow::LengthInPixel::LengthInPixel(MainWindow::LengthInPixel::ValueType value)
   : value(value)
{
}

MainWindow::LengthInPixel MainWindow::LengthInPixel::of(LengthInPixel::ValueType value)
{
   return LengthInPixel(value);
}

MainWindow::Size MainWindow::Size::ofPixel(LengthInPixel::ValueType width, LengthInPixel::ValueType height)
{
   return { LengthInPixel::of(width), LengthInPixel::of(height) };
}

MainWindow::Size::Size(MainWindow::LengthInPixel width, MainWindow::LengthInPixel height)
   : width(width)
   , height(height)
{
}

MainWindow::LengthInPixel MainWindow::Size::getWidth() const
{
   return width;
}

MainWindow::LengthInPixel MainWindow::Size::getHeight() const
{
   return height;
}

MainWindow::Size const MainWindow::DEFAULT_SIZE = MainWindow::Size::ofPixel(1280, 720);
char const MainWindow::DEFAULT_TITLE[] = "contomap";
// According to http://www.libpng.org/pub/png/spec/1.2/PNG-Structure.html#Chunk-naming-conventions ,
// the chunk type is ancillary (lower), private (lower), conforming (upper), safe-to-copy (lower).
std::array<char, 5> const MainWindow::PNG_MAP_TYPE { 'c', 'm', 'P', 'm', 0x00 };

MainWindow::MainWindow(DisplayEnvironment &environment, contomap::editor::View &view, contomap::editor::InputRequestHandler &inputRequestHandler)
   : mapCamera(std::make_shared<MapCamera::ImmediateGearbox>())
   , environment(environment)
   , view(view)
   , editBuffer(inputRequestHandler, mapCamera)
   , selectionDrawOffset(SpacialCoordinate::Offset::of(0.0f, 0.0f))
{
   mouseHandler = [this](MouseInput const &input) { handleMouseIdle(input); };
}

void MainWindow::init()
{
   auto initialSize = MainWindow::DEFAULT_SIZE;
   InitWindow(initialSize.getWidth().raw<int>(), initialSize.getHeight().raw<int>(), MainWindow::DEFAULT_TITLE);
}

void MainWindow::close()
{
   CloseWindow();
}

void MainWindow::closeRequested()
{
   environment.closeWindow();
}

void MainWindow::nextFrame()
{
   updateState();

   BeginDrawing();

   drawBackground();

   auto renderContext = RenderContext::fromCurrentState();
   {
      auto contentSize = renderContext.getContentSize();
      auto projection = mapCamera.beginProjection(contentSize);
      auto currentMousePos = GetMousePosition();
      auto focusCoordinate = projection.unproject(currentMousePos);
      auto lastFocusCoordinate = lastMousePos.has_value() ? projection.unproject(lastMousePos.value()) : focusCoordinate;
      lastMousePos = currentMousePos;

      processInput(renderContext, focusCoordinate, Vector2Subtract(focusCoordinate, lastFocusCoordinate));

      drawMap(focusCoordinate);
   }
   drawUserInterface(renderContext);

   EndDrawing();
}

void MainWindow::processInput(RenderContext const &context, Vector2, Vector2 focusDelta)
{
   if (currentDialog != nullptr)
   {
      return;
   }

   // TODO: probably needs some better checks here -> hotkey system
   // TODO: consider pinch zoom as well?
   if (GetMouseWheelMove() > 0.0f)
   {
      mapCamera.zoom(doubledRelative(true));
   }
   else if (GetMouseWheelMove() < 0.0f)
   {
      mapCamera.zoom(doubledRelative(false));
   }

   bool panLeft = IsKeyDown(KEY_LEFT);
   bool panUp = IsKeyDown(KEY_UP);
   bool panRight = IsKeyDown(KEY_RIGHT);
   bool panDown = IsKeyDown(KEY_DOWN);
   mapCamera.pan(panLeft, panUp, panRight, panDown);

   if (IsKeyPressed(KEY_HOME))
   {
      if (IsKeyDown(KEY_LEFT_CONTROL))
      {
         editBuffer.setViewScopeToDefault();
      }
      mapCamera.panTo(MapCamera::HOME_POSITION);
   }

   bool isInsertOperation = IsKeyReleased(KEY_INSERT) || IsKeyReleased(KEY_I);
   bool isAssociationContext = IsKeyDown(KEY_LEFT_SHIFT);
   if (isInsertOperation)
   {
      if (isAssociationContext)
      {
         editBuffer.newAssociationRequested(spacialCameraLocation());
      }
      else
      {
         openNewTopicDialog();
      }
   }
   if (IsKeyReleased(KEY_T))
   {
      openNewLocateTopicAndActDialog();
   }

   if (IsKeyReleased(KEY_L))
   {
      editBuffer.linkSelection();
   }

   if (IsKeyReleased(KEY_DELETE))
   {
      editBuffer.deleteSelection();
   }

   if (IsKeyPressed(KEY_S) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))
   {
      requestSave();
   }

   auto mousePos = GetMousePosition();
   auto barHeight = layout.buttonHeight() + layout.padding() + 2.0f;
   auto contentSize = context.getContentSize();
   MouseInput input {
      .buttonPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT),
      .buttonDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT),
      .ctrlDown = IsKeyDown(KEY_LEFT_CONTROL),
      .abortPressed = IsKeyPressed(KEY_ESCAPE),
      .pixelPos = mousePos,
      .worldMoveDelta = focusDelta,
      .overMap = (mousePos.y > barHeight) && (mousePos.y < (contentSize.y - barHeight)),
   };
   mouseHandler(input);
}

void MainWindow::updateState()
{
   auto frameTime = contomap::frontend::FrameTime::fromLastFrame();
   mapCamera.timePassed(frameTime);
}

void MainWindow::cycleSelectedOccurrence(bool forward)
{
   if (forward)
   {
      editBuffer.cycleSelectedOccurrenceForward();
   }
   else
   {
      editBuffer.cycleSelectedOccurrenceReverse();
   }
   panCameraToSelectedOccurrence();
}

void MainWindow::jumpToFirstOccurrenceOf(Identifier topicId)
{
   editBuffer.selectClosestOccurrenceOf(topicId);
   panCameraToSelectedOccurrence();
}

void MainWindow::panCameraToSelectedOccurrence()
{
   auto occurrence = Selections::firstOccurrenceFrom(view.ofSelection(), view.ofMap());
   if (occurrence.has_value())
   {
      auto newLocation = occurrence.value().get().getLocation().getSpacial().getAbsoluteReference();
      mapCamera.panTo(Vector2 { .x = newLocation.X(), .y = newLocation.Y() });
   }
}

void MainWindow::handleMouseIdle(MouseInput const &input)
{
   if (input.buttonPressed && input.overMap)
   {
      auto action = input.ctrlDown ? SelectionAction::Toggle : SelectionAction::Set;
      currentFocus.modifySelection(editBuffer, action);

      mouseHandler = [this](MouseInput const &nested) { handleMouseDownMoving(nested); };
      mouseHandler(input);
   }
}

void MainWindow::handleMouseDownMoving(MouseInput const &input)
{
   if (currentFocus.hasNoItem())
   {
      mapCamera.panTo(Vector2Subtract(mapCamera.getCurrentPosition(), input.worldMoveDelta));
   }
   else
   {
      selectionDrawOffset = selectionDrawOffset.plus(SpacialCoordinate::Offset::of(input.worldMoveDelta.x, input.worldMoveDelta.y));
   }
   bool done = false;
   if (!input.buttonDown)
   {
      if (Vector2Length(Vector2 { .x = selectionDrawOffset.X(), .y = selectionDrawOffset.Y() }) > 0.0f)
      {
         editBuffer.moveSelectionBy(selectionDrawOffset);
      }
      done = true;
   }
   else if (input.abortPressed)
   {
      done = true;
   }
   if (done)
   {
      selectionDrawOffset = SpacialCoordinate::Offset::of(0.0f, 0.0f);
      mouseHandler = [this](MouseInput const &nested) { handleMouseIdle(nested); };
   }
}

void MainWindow::drawBackground()
{
   ClearBackground(WHITE);
}

void MainWindow::drawMap(Vector2 focusCoordinate)
{
   DirectMapRenderer directMapRenderer;
   FocusInterceptor focusInterceptor(directMapRenderer, focusCoordinate);

   renderMap(focusInterceptor, view.ofSelection(), currentFocus, selectionDrawOffset);

   currentFocus = focusInterceptor.getNewFocus();
}

void MainWindow::renderMap(MapRenderer &renderer, contomap::editor::Selection const &selection, Focus const &focus, SpacialCoordinate::Offset selectionOffset)
{
   static Style const defaultStyle = Style()
                                        .with(Style::ColorType::Text, Style::Color { .red = 0x00, .green = 0x00, .blue = 0x00, .alpha = 0xFF })
                                        .with(Style::ColorType::Fill, Style::Color { .red = 0xE0, .green = 0xE0, .blue = 0xE0, .alpha = 0xFF })
                                        .with(Style::ColorType::Line, Style::Color { .red = 0x00, .green = 0x00, .blue = 0x00, .alpha = 0xFF });

   auto const &viewScope = view.ofViewScope();
   auto const &map = view.ofMap();

   Identifiers associationIds;
   std::map<Identifier, Vector2> associationLocationsById;
   auto drawOffsetIf = [&selectionOffset](bool isSelected) { return isSelected ? selectionOffset : SpacialCoordinate::Offset::of(0.0f, 0.0f); };

   auto visibleAssociations = map.find(Associations::thatAreIn(viewScope));
   for (Association const &visibleAssociation : visibleAssociations)
   {
      bool associationIsSelected = selection.contains(SelectedType::Association, visibleAssociation.getId());
      auto optionalTypeId = visibleAssociation.getType();
      std::string nameText;
      if (optionalTypeId.isAssigned())
      {
         auto typeTopic = view.ofMap().findTopic(optionalTypeId.value());
         nameText = bestTitleFor(typeTopic.value());
      }

      auto spacialLocation = visibleAssociation.getLocation().getSpacial().getAbsoluteReference().plus(drawOffsetIf(associationIsSelected));
      Vector2 projectedLocation { .x = spacialLocation.X(), .y = spacialLocation.Y() };

      Font font = GetFontDefault();
      float fontSize = 16.0f;
      float spacing = 1.0f;
      auto textSize = MeasureTextEx(font, nameText.c_str(), fontSize, spacing);
      float lineThickness = 2.0f;

      Rectangle textArea {
         .x = projectedLocation.x - textSize.x / 2.0f,
         .y = projectedLocation.y - textSize.y / 2.0f,
         .width = textSize.x,
         .height = textSize.y,
      };

      float platePadding = 2.0f;
      Rectangle plate {
         .x = textArea.x - platePadding,
         .y = textArea.y - platePadding,
         .width = textArea.width + platePadding * 2.0f,
         .height = textArea.height + platePadding * 2.0f,
      };
      float halfHeight = plate.height / 2.0f;
      float reifierPadding = 2.0f + (lineThickness * 0.4f);
      float reifierOffset = lineThickness + reifierPadding;
      Rectangle area {
         .x = plate.x - reifierOffset - lineThickness - halfHeight,
         .y = plate.y - lineThickness,
         .width = plate.width + (reifierOffset + lineThickness + halfHeight) * 2.0f,
         .height = plate.height + lineThickness * 2.0f,
      };

      associationIds.add(visibleAssociation.getId());
      associationLocationsById[visibleAssociation.getId()] = projectedLocation;

      auto associationStyle
         = Styles::resolve(visibleAssociation.getAppearance(), visibleAssociation.getType(), view.ofViewScope(), view.ofMap()).withDefaultsFrom(defaultStyle);
      if (associationIsSelected)
      {
         associationStyle = selectedStyle(associationStyle);
      }
      if (focus.isAssociation(visibleAssociation.getId()))
      {
         associationStyle = highlightedStyle(associationStyle);
      }

      renderer.renderAssociationPlate(visibleAssociation.getId(), area, associationStyle, plate, lineThickness, visibleAssociation.hasReifier());
      renderer.renderText(textArea, Style().with(Style::ColorType::Text, associationStyle.get(Style::ColorType::Text)), nameText, font, fontSize, spacing);
   }

   auto visibleTopics = map.find(Topics::thatAreIn(viewScope));
   for (Topic const &visibleTopic : visibleTopics)
   {
      std::string nameText = bestTitleFor(visibleTopic);
      std::vector<std::reference_wrapper<Role const>> roles;
      for (Role const &role : visibleTopic.rolesAssociatedWith(associationIds))
      {
         roles.emplace_back(role);
      }

      for (Occurrence const &occurrence : visibleTopic.occurrencesIn(viewScope))
      {
         bool occurrenceIsSelected = selection.contains(SelectedType::Occurrence, occurrence.getId());
         auto spacialLocation = occurrence.getLocation().getSpacial().getAbsoluteReference().plus(drawOffsetIf(occurrenceIsSelected));
         Vector2 projectedLocation { .x = spacialLocation.X(), .y = spacialLocation.Y() };

         for (Role const &role : roles)
         {
            bool roleIsSelected = selection.contains(SelectedType::Role, role.getId());
            std::string roleTitle;
            auto optionalTypeId = role.getType();
            if (optionalTypeId.isAssigned())
            {
               auto typeTopic = view.ofMap().findTopic(optionalTypeId.value());
               roleTitle = bestTitleFor(typeTopic.value());
            }

            auto roleStyle = Styles::resolve(role.getAppearance(), role.getType(), view.ofViewScope(), view.ofMap()).withDefaultsFrom(defaultStyle);

            float thickness = 1.0f;
            if (roleIsSelected)
            {
               roleStyle = selectedStyle(roleStyle);
               thickness += 2.0f;
            }
            if (focus.isRole(role.getId()))
            {
               roleStyle = highlightedStyle(roleStyle);
               thickness += 0.5f;
            }

            auto associationLocation = associationLocationsById[role.getParent()];
            renderer.renderRoleLine(role.getId(), projectedLocation, associationLocation, roleStyle, thickness, role.hasReifier());

            if (!roleTitle.empty())
            {
               Font font = GetFontDefault();
               float fontSize = 10.0f;
               float spacing = 1.0f;
               auto textSize = MeasureTextEx(font, roleTitle.c_str(), fontSize, spacing);
               float plateHeight = textSize.y;

               Rectangle area {
                  .x = (projectedLocation.x + associationLocation.x) / 2,
                  .y = (projectedLocation.y + associationLocation.y) / 2 - textSize.y / 2.0f,
                  .width = textSize.x,
                  .height = plateHeight,
               };

               renderer.renderText(area, roleStyle.without(Style::ColorType::Line), roleTitle, font, fontSize, spacing);
            }
         }

         Font font = GetFontDefault();
         float fontSize = 16.0f;
         float spacing = 1.0f;
         auto textSize = MeasureTextEx(font, nameText.c_str(), fontSize, spacing);

         float lineThickness = 2.0f;

         Rectangle textArea {
            .x = projectedLocation.x - textSize.x / 2.0f,
            .y = projectedLocation.y - textSize.y / 2.0f,
            .width = textSize.x,
            .height = textSize.y,
         };
         float platePadding = 2.0f;
         Rectangle plate {
            .x = textArea.x - platePadding,
            .y = textArea.y - platePadding,
            .width = textArea.width + platePadding * 2.0f,
            .height = textArea.height + platePadding * 2.0f,
         };
         float reifierPadding = 2.0f;
         float reifierOffset = lineThickness + reifierPadding;
         Rectangle area {
            .x = plate.x - reifierOffset - lineThickness,
            .y = plate.y - lineThickness,
            .width = plate.width + (reifierOffset * 2.0f) + (lineThickness * 2.0f),
            .height = plate.height + (lineThickness * 2.0f),
         };

         auto occurrenceStyle
            = Styles::resolve(occurrence.getAppearance(), occurrence.getType(), view.ofViewScope(), view.ofMap()).withDefaultsFrom(defaultStyle);
         if (occurrenceIsSelected)
         {
            occurrenceStyle = selectedStyle(occurrenceStyle);
         }
         if (focus.isOccurrence(occurrence.getId()))
         {
            occurrenceStyle = highlightedStyle(occurrenceStyle);
         }

         renderer.renderOccurrencePlate(occurrence.getId(), area, occurrenceStyle, plate, lineThickness, occurrence.hasReifier());
         renderer.renderText(textArea, Style().with(Style::ColorType::Text, occurrenceStyle.get(Style::ColorType::Text)), nameText, font, fontSize, spacing);
      }
   }
}

void MainWindow::drawUserInterface(RenderContext const &context)
{
   if (pendingDialog != nullptr)
   {
      currentDialog = std::move(pendingDialog);
   }
   if (currentDialog != nullptr)
   {
      GuiLock();
      GuiDisableTooltip();
   }
   else
   {
      GuiUnlock();
      GuiEnableTooltip();
   }

   auto contentSize = context.getContentSize();
   auto iconSize = layout.buttonHeight();
   auto padding = layout.padding();
   {
      Vector2 toolbarPosition { .x = 0, .y = 0 };
      Vector2 toolbarSize { .x = contentSize.x, .y = iconSize + (padding * 2.0f) };
      GuiPanel(Rectangle { toolbarPosition.x, toolbarPosition.y, toolbarSize.x, toolbarSize.y }, nullptr);
      GuiSetTooltip("Show help window");
      if (GuiButton(
             Rectangle {
                .x = toolbarPosition.x + toolbarSize.x - (padding + iconSize), .y = toolbarPosition.y + padding, .width = iconSize, .height = iconSize },
             GuiIconText(ICON_HELP, nullptr)))
      {
         openHelpDialog();
      }

      Rectangle leftIconButtonsBounds {
         .x = toolbarPosition.x + padding,
         .y = toolbarPosition.y + padding,
         .width = iconSize,
         .height = iconSize,
      };
      GuiSetTooltip("New map");
      if (GuiButton(leftIconButtonsBounds, GuiIconText(ICON_FILE_NEW, nullptr)))
      {
         requestNewFile();
      }
      leftIconButtonsBounds.x += (iconSize + padding);
      GuiSetTooltip("Load map");
      if (GuiButton(leftIconButtonsBounds, GuiIconText(ICON_FILE_OPEN, nullptr)))
      {
         requestLoad();
      }
      leftIconButtonsBounds.x += (iconSize + padding);
      GuiSetTooltip("Save map");
      if (GuiButton(leftIconButtonsBounds, GuiIconText(ICON_FILE_SAVE, nullptr)))
      {
         requestSave();
      }
      leftIconButtonsBounds.x += (iconSize + padding);

      leftIconButtonsBounds.x += (iconSize + padding);
      if (!editBuffer.canUndo())
      {
         GuiDisable();
      }
      GuiSetTooltip("Undo last operation");
      if (GuiButton(leftIconButtonsBounds, GuiIconText(ICON_UNDO, nullptr)))
      {
         editBuffer.undo();
      }
      GuiEnable();
      leftIconButtonsBounds.x += (iconSize + padding);
      if (!editBuffer.canRedo())
      {
         GuiDisable();
      }
      GuiSetTooltip("Redo last operation");
      if (GuiButton(leftIconButtonsBounds, GuiIconText(ICON_REDO, nullptr)))
      {
         editBuffer.redo();
      }
      GuiEnable();
      leftIconButtonsBounds.x += (iconSize + padding);

      leftIconButtonsBounds.x += (iconSize + padding);
      GuiSetTooltip("Set home view scope");
      if (GuiButton(leftIconButtonsBounds, GuiIconText(ICON_HOUSE, nullptr)))
      {
         editBuffer.setViewScopeToDefault();
      }
      leftIconButtonsBounds.x += (iconSize + padding);
      GuiSetTooltip("Set view scope to selected");
      if (GuiButton(leftIconButtonsBounds, GuiIconText(ICON_BOX_DOTS_BIG, nullptr)))
      {
         editBuffer.setViewScopeFromSelection();
      }
      leftIconButtonsBounds.x += (iconSize + padding);
      GuiSetTooltip("Add selected to view scope");
      if (GuiButton(leftIconButtonsBounds, "+v"))
      {
         editBuffer.addToViewScopeFromSelection();
      }
      leftIconButtonsBounds.x += (iconSize + padding);
      GuiSetTooltip("Cycle to previous occurrence");
      if (!view.ofSelection().hasSoleEntryFor(SelectedType::Occurrence))
      {
         GuiDisable();
      }
      if (GuiButton(leftIconButtonsBounds, GuiIconText(ICON_ARROW_LEFT_FILL, nullptr)))
      {
         cycleSelectedOccurrence(false);
      }
      leftIconButtonsBounds.x += (iconSize + padding);
      GuiSetTooltip("Cycle to next occurrence");
      if (GuiButton(leftIconButtonsBounds, GuiIconText(ICON_ARROW_RIGHT_FILL, nullptr)))
      {
         cycleSelectedOccurrence(true);
      }
      GuiEnable();

      leftIconButtonsBounds.x += (iconSize + padding) * 2;
      if (!view.ofSelection().hasSoleEntryFor(SelectedType::Occurrence))
      {
         GuiDisable();
      }
      GuiSetTooltip("Set topic default name");
      if (GuiButton(leftIconButtonsBounds, "N"))
      {
         openSetTopicNameDefaultDialog();
      }
      leftIconButtonsBounds.x += (iconSize + padding);
      GuiSetTooltip("Set topic scoped name");
      if (GuiButton(leftIconButtonsBounds, "[N]"))
      {
         openSetTopicNameInScopeDialog();
      }
      GuiEnable();
      leftIconButtonsBounds.x += (iconSize + padding);

      leftIconButtonsBounds.x += (iconSize + padding);
      if (view.ofSelection().empty())
      {
         GuiDisable();
      }
      GuiSetTooltip("Set appearance");
      if (GuiButton(leftIconButtonsBounds, GuiIconText(ICON_BRUSH_CLASSIC, nullptr)))
      {
         openEditStyleDialog();
      }
      GuiEnable();
      leftIconButtonsBounds.x += (iconSize + padding);

      GuiSetTooltip("Clear type");
      if (view.ofSelection().empty())
      {
         GuiDisable();
      }
      if (GuiButton(leftIconButtonsBounds, "-T"))
      {
         editBuffer.clearTypeOfSelection();
      }
      GuiEnable();
      leftIconButtonsBounds.x += (iconSize + padding);

      leftIconButtonsBounds.x += (iconSize + padding);
      GuiSetTooltip("Go to reifier");
      if (!view.ofSelection().hasSoleEntry() || !Selections::firstReifiableFrom(view.ofSelection(), view.ofMap()).value().get().getReifier().has_value())
      {
         GuiDisable();
      }
      if (GuiButton(leftIconButtonsBounds, "[R]"))
      {
         auto optionalReifiable = Selections::firstReifiableFrom(view.ofSelection(), view.ofMap());
         if (optionalReifiable.has_value())
         {
            contomap::model::Reifiable<Topic> const &reifiable = optionalReifiable.value();
            jumpToFirstOccurrenceOf(reifiable.getReifier().value().get().getId());
         }
      }
      GuiEnable();
      leftIconButtonsBounds.x += (iconSize + padding);
      GuiSetTooltip("Clear reifier");
      if (view.ofSelection().empty())
      {
         GuiDisable();
      }
      if (GuiButton(leftIconButtonsBounds, "-R"))
      {
         editBuffer.clearReifierOfSelection();
      }
      GuiEnable();
   }

   {
      Vector2 viewScopeSize { .x = contentSize.x, .y = iconSize + (padding * 2.0f) };
      Vector2 viewScopePosition { .x = 0, .y = contentSize.y - viewScopeSize.y };
      Rectangle viewScopeBounds { .x = viewScopePosition.x, .y = viewScopePosition.y, .width = viewScopeSize.x, .height = viewScopeSize.y };
      GuiPanel(viewScopeBounds, nullptr);
      float buttonStartX = viewScopePosition.x + padding;

      auto viewScope = view.ofViewScope();
      if (lastViewScope != viewScope)
      {
         viewScopeListStartIndex = 0;
         lastViewScope = viewScope;
      }

      GuiSetTooltip("Scroll view scope left");
      if (viewScopeListStartIndex == 0)
      {
         GuiDisable();
      }
      if (GuiButton(
             Rectangle { .x = buttonStartX, .y = viewScopePosition.y + padding, .width = iconSize, .height = iconSize }, GuiIconText(ICON_ARROW_LEFT, nullptr)))
      {
         if (viewScopeListStartIndex > 0)
         {
            viewScopeListStartIndex--;
         }
      }
      GuiEnable();
      buttonStartX += iconSize + padding;
      GuiSetTooltip("Scroll view scope right");
      if ((viewScopeListStartIndex + 1) >= viewScope.size())
      {
         viewScopeListStartIndex = viewScope.size() - 1;
         GuiDisable();
      }
      if (GuiButton(
             Rectangle {
                .x = viewScopePosition.x + padding + (iconSize + padding) * 1, .y = viewScopePosition.y + padding, .width = iconSize, .height = iconSize },
             GuiIconText(ICON_ARROW_RIGHT, nullptr)))
      {
         viewScopeListStartIndex++;
      }
      GuiEnable();
      buttonStartX += iconSize + padding;

      auto add = [this, &buttonStartX, viewScopePosition, padding, iconSize](Identifier id, std::string const &name) {
         Font font = GuiGetFont();
         auto fontSize = static_cast<float>(GuiGetStyle(DEFAULT, TEXT_SIZE));
         auto spacing = static_cast<float>(GuiGetStyle(DEFAULT, TEXT_SPACING));
         auto textPadding = static_cast<float>(GuiGetStyle(CHECKBOX, TEXT_PADDING));
         auto textSize = MeasureTextEx(font, name.c_str(), fontSize, spacing);

         float labelWidth = textSize.x + textPadding * 2;
         bool isActive = true;
         GuiToggle(Rectangle { .x = buttonStartX, .y = viewScopePosition.y + padding, .width = labelWidth, .height = iconSize }, name.c_str(), &isActive);
         if (!isActive)
         {
            editBuffer.removeFromViewScope(id);
         }
         buttonStartX += labelWidth + padding;
      };

      size_t scopeIndex = 0;
      for (auto id : viewScope)
      {
         scopeIndex++;
         if (scopeIndex <= viewScopeListStartIndex)
         {
            continue;
         }
         auto const &topic = view.ofMap().findTopic(id);
         add(id, topic.has_value() ? bestTitleFor(topic.value()) : "???");
      }
   }

   if (currentDialog != nullptr)
   {
      DrawRectangle(0, 0, static_cast<int>(contentSize.x), static_cast<int>(contentSize.y), Fade(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), 0.8f));
      GuiUnlock();

      if (currentDialog->draw(context))
      {
         closeDialog();
      }
   }
}

void MainWindow::requestNewFile()
{
   editBuffer.newMap();
   mapRestored("");
}

void MainWindow::requestLoad()
{
   pendingDialog = std::make_unique<contomap::frontend::LoadDialog>(environment, layout, [this](std::string const &filePath) { load(filePath); });
}

void MainWindow::requestSave()
{
   if (!currentFilePath.empty())
   {
      save();
   }
   else
   {
      pendingDialog = std::make_unique<contomap::frontend::SaveAsDialog>(environment, layout, "unnamed.contomap.png", [this](std::string const &filePath) {
         currentFilePath = filePath;
         save();
      });
   }
}

void MainWindow::closeDialog()
{
   currentDialog.reset();
}

void MainWindow::openHelpDialog()
{
   pendingDialog = std::make_unique<contomap::frontend::HelpDialog>(layout);
}

void MainWindow::openNewTopicDialog()
{
   auto cameraLocation = spacialCameraLocation();
   std::vector<LocateTopicAndActDialog::TitledAction> actions {
      LocateTopicAndActDialog::newTopic(cameraLocation),
      LocateTopicAndActDialog::newOccurrence(cameraLocation),
   };
   pendingDialog = std::make_unique<LocateTopicAndActDialog>(editBuffer, view.ofMap(), layout, actions);
}

void MainWindow::openNewLocateTopicAndActDialog()
{
   auto cameraLocation = spacialCameraLocation();
   std::vector<LocateTopicAndActDialog::TitledAction> actions {
      LocateTopicAndActDialog::setViewScope(),
      LocateTopicAndActDialog::addToViewScope(),
      LocateTopicAndActDialog::newTopic(cameraLocation),
      LocateTopicAndActDialog::newOccurrence(cameraLocation),
   };
   if (!view.ofSelection().empty())
   {
      actions.emplace_back(LocateTopicAndActDialog::setTypeOfSelection());
      actions.emplace_back(LocateTopicAndActDialog::setNewTypeOfSelection());
   }
   if (view.ofSelection().hasSoleEntry())
   {
      actions.emplace_back(LocateTopicAndActDialog::setReifierOfSelection());
      actions.emplace_back(LocateTopicAndActDialog::setNewReifierOfSelection());
   }
   pendingDialog = std::make_unique<LocateTopicAndActDialog>(editBuffer, view.ofMap(), layout, actions);
}

void MainWindow::openSetTopicNameDefaultDialog()
{
   auto topic = Selections::topicOfFirstOccurrenceFrom(view.ofSelection(), view.ofMap());
   if (!topic.has_value())
   {
      return;
   }

   pendingDialog = RenameTopicDialog::forDefaultName(editBuffer, layout, topic.value().get().getId());
}

void MainWindow::openSetTopicNameInScopeDialog()
{
   auto topic = Selections::topicOfFirstOccurrenceFrom(view.ofSelection(), view.ofMap());
   if (!topic.has_value())
   {
      return;
   }

   pendingDialog = RenameTopicDialog::forScopedName(editBuffer, layout, topic.value().get().getId());
}

void MainWindow::openEditStyleDialog()
{
   auto style = Selections::firstAppearanceFrom(view.ofSelection(), view.ofMap());
   if (!style.has_value())
   {
      return;
   }

   pendingDialog = std::make_unique<contomap::frontend::StyleDialog>(editBuffer, layout, style.value());
}

void MainWindow::load(std::string const &filePath)
{
   auto chunk = rpng_chunk_read(filePath.c_str(), PNG_MAP_TYPE.data());
   if (chunk.length == 0)
   {
      return;
   }
   contomap::infrastructure::serial::BinaryDecoder decoder(chunk.data, chunk.data + chunk.length);
   if (editBuffer.loadState(decoder))
   {
      mapRestored(filePath);
   }

   RPNG_FREE(chunk.data);
}

void MainWindow::save()
{
   contomap::frontend::MapRenderList renderList;
   renderMap(renderList, {}, {}, SpacialCoordinate::Offset::of(0.0f, 0.0f));
   contomap::frontend::MapRenderMeasurer measurer;
   renderList.renderTo(measurer);
   auto mapArea = measurer.getArea();
   mapArea.x -= 5.0f;
   mapArea.y -= 5.0f;
   mapArea.width += 10.0f;
   mapArea.height += 10.0f;
   // The DPI scale is also considered when rendering to texture, so increase its size accordingly.
   auto dpiScale = GetWindowScaleDPI();
   auto renderTexture = LoadRenderTexture(std::ceil(mapArea.width * dpiScale.x), std::ceil(mapArea.height * dpiScale.y));

   {
      DirectMapRenderer directRenderer;
      BeginTextureMode(renderTexture);
      drawBackground();
      MapCamera camera(std::make_unique<MapCamera::ImmediateGearbox>());
      camera.panTo(Vector2 { .x = mapArea.x + (mapArea.width / 2.0f), .y = mapArea.y + (mapArea.height / 2.0f) });
      auto projection = camera.beginProjection(Vector2 { mapArea.width, mapArea.height });
      renderList.renderTo(directRenderer);
      EndTextureMode();
   }

   auto image = LoadImageFromTexture(renderTexture.texture);
   ImageFlipVertical(&image);
   int fileSize = 0;
   auto exported = ExportImageToMemory(image, ".png", &fileSize);
   UnloadImage(image);
   UnloadRenderTexture(renderTexture);
   if (exported == nullptr)
   {
      return;
   }
   {
      int outputSize = 0;
      contomap::infrastructure::serial::BinaryEncoder encoder;
      rpng_chunk chunk;
      memset(&chunk, 0x00, sizeof(chunk));
      editBuffer.saveState(encoder, false);
      auto const &data = encoder.getData();
      chunk.data = const_cast<uint8_t *>(data.data());
      chunk.length = static_cast<int>(data.size());
      memcpy(chunk.type, PNG_MAP_TYPE.data(), 4);

      auto newExported = rpng_chunk_write_from_memory(reinterpret_cast<char const *>(exported), chunk, &outputSize);
      RL_FREE(exported);
      exported = reinterpret_cast<uint8_t *>(newExported);
      fileSize = outputSize;
   }
   if (exported != nullptr)
   {
      SaveFileData(currentFilePath.c_str(), exported, fileSize);
      RPNG_FREE(exported);

      environment.fileSaved(currentFilePath);
   }
}

void MainWindow::mapRestored(std::string const &filePath)
{
   currentFilePath = filePath;
   currentFocus = contomap::frontend::Focus();
   mapCamera.panTo(MapCamera::HOME_POSITION);
   lastViewScope.clear();
   viewScopeListStartIndex = 0;
}

SpacialCoordinate MainWindow::spacialCameraLocation()
{
   auto centerPoint = mapCamera.getCurrentPosition();
   return contomap::model::SpacialCoordinate::absoluteAt(centerPoint.x, centerPoint.y);
}

std::vector<std::pair<int, MapCamera::ZoomFactor>> MainWindow::generateZoomLevels()
{
   std::vector<std::pair<int, MapCamera::ZoomFactor>> levels;
   int stepSize = 5;
   for (int i = -20; i < 40; i += stepSize)
   {
      levels.emplace_back(i, MapCamera::ZoomFactor::from(std::pow(2.0f, static_cast<float>(i) / 10.0f)));
   }
   return levels;
}

MapCamera::ZoomOperation MainWindow::doubledRelative(bool nearer)
{
   static std::vector<std::pair<int, contomap::frontend::MapCamera::ZoomFactor>> const ZOOM_LEVELS(MainWindow::generateZoomLevels());

   return [nearer](MapCamera::ZoomFactor currentTarget) {
      float currentRawTarget = currentTarget.raw();
      if (nearer)
      {
         auto it = std::find_if(ZOOM_LEVELS.begin(), ZOOM_LEVELS.end(), [currentRawTarget](auto pair) { return pair.second.raw() > currentRawTarget; });
         return (it != ZOOM_LEVELS.end()) ? it->second : ZOOM_LEVELS[ZOOM_LEVELS.size() - 1].second;
      }
      else
      {
         auto it = std::find_if(ZOOM_LEVELS.rbegin(), ZOOM_LEVELS.rend(), [currentRawTarget](auto pair) { return pair.second.raw() < currentRawTarget; });
         return (it != ZOOM_LEVELS.rend()) ? it->second : ZOOM_LEVELS[0].second;
      }
   };
}

std::string MainWindow::bestTitleFor(Topic const &topic)
{
   return Names::forScopedDisplay(topic, view.ofViewScope(), view.ofMap().getDefaultScope())[0];
}

Style MainWindow::selectedStyle(Style style)
{
   float factor = 0.5f;
   Style copy = std::move(style);
   return copy.with(Style::ColorType::Fill, brightenColor(copy.get(Style::ColorType::Fill), factor))
      .with(Style::ColorType::Line, brightenColor(copy.get(Style::ColorType::Line), factor));
}

Style MainWindow::highlightedStyle(Style style)
{
   float factor = 0.75f;
   Style copy = std::move(style);
   return copy.with(Style::ColorType::Fill, brightenColor(copy.get(Style::ColorType::Fill), factor))
      .with(Style::ColorType::Line, brightenColor(copy.get(Style::ColorType::Line), factor));
}

Style::Color MainWindow::brightenColor(Style::Color base, float factor)
{
   return Colors::fromUiColor(ColorBrightness(Colors::toUiColor(base), factor));
}
