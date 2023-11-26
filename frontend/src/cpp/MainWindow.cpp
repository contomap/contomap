#include <cmath>
#include <sstream>

#include <raygui/raygui.h>

#include "contomap/editor/Selections.h"
#include "contomap/editor/Styles.h"
#include "contomap/frontend/Colors.h"
#include "contomap/frontend/DirectMapRenderer.h"
#include "contomap/frontend/FocusInterceptor.h"
#include "contomap/frontend/HelpDialog.h"
#include "contomap/frontend/LocateTopicAndActDialog.h"
#include "contomap/frontend/MainWindow.h"
#include "contomap/frontend/Names.h"
#include "contomap/frontend/NewTopicDialog.h"
#include "contomap/frontend/RenameTopicDialog.h"
#include "contomap/frontend/SaveAsDialog.h"
#include "contomap/frontend/StyleDialog.h"
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
std::vector<std::pair<int, contomap::frontend::MapCamera::ZoomFactor>> const MainWindow::ZOOM_LEVELS(MainWindow::generateZoomLevels());

MainWindow::MainWindow(DisplayEnvironment &environment, contomap::editor::View &view, contomap::editor::InputRequestHandler &inputRequestHandler)
   : mapCamera(std::make_shared<MapCamera::ImmediateGearbox>())
   , environment(environment)
   , view(view)
   , inputRequestHandler(inputRequestHandler)
{
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
   processInput();
   updateState();

   BeginDrawing();

   auto renderContext = RenderContext::fromCurrentState();

   drawBackground();
   drawMap(renderContext);
   drawUserInterface(renderContext);

   EndDrawing();
}

void MainWindow::processInput()
{
   if (currentDialog == nullptr)
   {
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
            inputRequestHandler.setViewScopeToDefault();
         }
         mapCamera.panTo(MapCamera::HOME_POSITION);
      }

      bool isInsertOperation = IsKeyReleased(KEY_INSERT) || IsKeyReleased(KEY_I);
      bool isAssociationContext = IsKeyDown(KEY_LEFT_SHIFT);
      if (isInsertOperation)
      {
         if (isAssociationContext)
         {
            inputRequestHandler.newAssociationRequested(spacialCameraLocation());
         }
         else
         {
            openNewTopicDialog();
         }
      }
      if (IsKeyReleased(KEY_O))
      {
         openNewOccurrenceDialog();
      }
      if (IsKeyReleased(KEY_T))
      {
         openNewLocateTopicAndActDialog();
      }

      if (IsKeyReleased(KEY_L))
      {
         inputRequestHandler.linkSelection();
      }

      if (IsKeyReleased(KEY_DELETE))
      {
         inputRequestHandler.deleteSelection();
      }

      // TODO: avoid map interaction click when on view scope bar.
      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && (static_cast<float>(GetMouseY()) > (layout.buttonHeight() + layout.padding() * 2.0f)))
      {
         auto action = IsKeyDown(KEY_LEFT_CONTROL) ? SelectionAction::Toggle : SelectionAction::Set;
         currentFocus.modifySelection(inputRequestHandler, action);
      }
   }
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
      inputRequestHandler.cycleSelectedOccurrenceForward();
   }
   else
   {
      inputRequestHandler.cycleSelectedOccurrenceReverse();
   }
   panCameraToSelectedOccurrence();
}

void MainWindow::jumpToFirstOccurrenceOf(Identifier topicId)
{
   inputRequestHandler.selectClosestOccurrenceOf(topicId);
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

void MainWindow::drawBackground()
{
   ClearBackground(WHITE);
}

void MainWindow::drawMap(RenderContext const &context)
{
   static Style const defaultStyle = Style()
                                        .with(Style::ColorType::Text, Style::Color { .red = 0x00, .green = 0x00, .blue = 0x00, .alpha = 0xFF })
                                        .with(Style::ColorType::Fill, Style::Color { .red = 0xE0, .green = 0xE0, .blue = 0xE0, .alpha = 0xFF })
                                        .with(Style::ColorType::Line, Style::Color { .red = 0x00, .green = 0x00, .blue = 0x00, .alpha = 0xFF });

   auto contentSize = context.getContentSize();
   auto projection = mapCamera.beginProjection(contentSize);
   auto focusCoordinate = projection.unproject(GetMousePosition());

   DirectMapRenderer directMapRenderer;
   FocusInterceptor focusInterceptor(directMapRenderer, focusCoordinate);
   MapRenderer &renderer = focusInterceptor;

   auto const &viewScope = view.ofViewScope();
   auto const &map = view.ofMap();
   auto const &selection = view.ofSelection();

   // TODO: rework algorithm: need first to determine visible/referenced topics & associations; declutter; draw player lines; draw topics; animate!

   Identifiers associationIds;
   std::map<Identifier, Vector2> associationLocationsById;

   auto visibleAssociations = map.find(Associations::thatAreIn(viewScope));
   for (Association const &visibleAssociation : visibleAssociations)
   {
      auto optionalTypeId = visibleAssociation.getType();
      std::string nameText;
      if (optionalTypeId.isAssigned())
      {
         auto typeTopic = view.ofMap().findTopic(optionalTypeId.value());
         nameText = bestTitleFor(typeTopic.value());
      }

      auto spacialLocation = visibleAssociation.getLocation().getSpacial().getAbsoluteReference();
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
      if (selection.contains(SelectedType::Association, visibleAssociation.getId()))
      {
         associationStyle = selectedStyle(associationStyle);
      }
      if (currentFocus.isAssociation(visibleAssociation.getId()))
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
         auto spacialLocation = occurrence.getLocation().getSpacial().getAbsoluteReference();
         Vector2 projectedLocation { .x = spacialLocation.X(), .y = spacialLocation.Y() };

         for (Role const &role : roles)
         {
            std::string roleTitle;
            auto optionalTypeId = role.getType();
            if (optionalTypeId.isAssigned())
            {
               auto typeTopic = view.ofMap().findTopic(optionalTypeId.value());
               roleTitle = bestTitleFor(typeTopic.value());
            }

            auto roleStyle = Styles::resolve(role.getAppearance(), role.getType(), view.ofViewScope(), view.ofMap()).withDefaultsFrom(defaultStyle);

            float thickness = 1.0f;
            if (selection.contains(SelectedType::Role, role.getId()))
            {
               roleStyle = selectedStyle(roleStyle);
               thickness += 2.0f;
            }
            if (currentFocus.isRole(role.getId()))
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
         if (selection.contains(SelectedType::Occurrence, occurrence.getId()))
         {
            occurrenceStyle = selectedStyle(occurrenceStyle);
         }
         if (currentFocus.isOccurrence(occurrence.getId()))
         {
            occurrenceStyle = highlightedStyle(occurrenceStyle);
         }

         renderer.renderOccurrencePlate(occurrence.getId(), area, occurrenceStyle, plate, lineThickness, occurrence.hasReifier());
         renderer.renderText(textArea, Style().with(Style::ColorType::Text, occurrenceStyle.get(Style::ColorType::Text)), nameText, font, fontSize, spacing);
      }
   }

   currentFocus = focusInterceptor.getNewFocus();
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
      GuiSetTooltip("Save map");
      if (GuiButton(leftIconButtonsBounds, GuiIconText(ICON_FILE_SAVE, nullptr)))
      {
         requestSave();
      }
      leftIconButtonsBounds.x += (iconSize + padding);

      leftIconButtonsBounds.x += (iconSize + padding);
      GuiSetTooltip("Set home view scope");
      if (GuiButton(leftIconButtonsBounds, GuiIconText(ICON_HOUSE, nullptr)))
      {
         inputRequestHandler.setViewScopeToDefault();
         mapCamera.panTo(MapCamera::HOME_POSITION);
      }
      leftIconButtonsBounds.x += (iconSize + padding);
      GuiSetTooltip("Set view scope to selected");
      if (GuiButton(leftIconButtonsBounds, GuiIconText(ICON_BOX_DOTS_BIG, nullptr)))
      {
         inputRequestHandler.setViewScopeFromSelection();
         mapCamera.panTo(MapCamera::HOME_POSITION);
      }
      leftIconButtonsBounds.x += (iconSize + padding);
      GuiSetTooltip("Add selected to view scope");
      if (GuiButton(leftIconButtonsBounds, "+v"))
      {
         inputRequestHandler.addToViewScopeFromSelection();
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
         inputRequestHandler.clearTypeOfSelection();
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
         inputRequestHandler.clearReifierOfSelection();
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
            inputRequestHandler.removeFromViewScope(id);
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

void MainWindow::requestSave()
{
   // TODO: check for existing name, then save directly
   pendingDialog = std::make_unique<contomap::frontend::SaveAsDialog>(
      environment, layout, "unnamed.contomap.png", [this](std::string const &filePath) { saveAs(filePath); });
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
   pendingDialog = std::make_unique<contomap::frontend::NewTopicDialog>(inputRequestHandler, layout, spacialCameraLocation());
}

void MainWindow::openNewOccurrenceDialog()
{
   std::vector<LocateTopicAndActDialog::TitledAction> actions { LocateTopicAndActDialog::newOccurrence(spacialCameraLocation()) };
   pendingDialog = std::make_unique<LocateTopicAndActDialog>(inputRequestHandler, view.ofMap(), layout, actions);
}

void MainWindow::openNewLocateTopicAndActDialog()
{
   std::vector<LocateTopicAndActDialog::TitledAction> actions {
      LocateTopicAndActDialog::setViewScope(),
      LocateTopicAndActDialog::addToViewScope(),
      LocateTopicAndActDialog::newOccurrence(spacialCameraLocation()),

   };
   if (!view.ofSelection().empty())
   {
      actions.emplace_back(LocateTopicAndActDialog::setTypeOfSelection());
   }
   if (view.ofSelection().hasSoleEntry())
   {
      actions.emplace_back(LocateTopicAndActDialog::setReifierOfSelection());
   }
   pendingDialog = std::make_unique<LocateTopicAndActDialog>(inputRequestHandler, view.ofMap(), layout, actions);
}

void MainWindow::openSetTopicNameDefaultDialog()
{
   auto topic = Selections::topicOfFirstOccurrenceFrom(view.ofSelection(), view.ofMap());
   if (!topic.has_value())
   {
      return;
   }

   pendingDialog = RenameTopicDialog::forDefaultName(inputRequestHandler, layout, topic.value().get().getId());
}

void MainWindow::openSetTopicNameInScopeDialog()
{
   auto topic = Selections::topicOfFirstOccurrenceFrom(view.ofSelection(), view.ofMap());
   if (!topic.has_value())
   {
      return;
   }

   pendingDialog = RenameTopicDialog::forScopedName(inputRequestHandler, layout, topic.value().get().getId());
}

void MainWindow::openEditStyleDialog()
{
   auto style = Selections::firstAppearanceFrom(view.ofSelection(), view.ofMap());
   if (!style.has_value())
   {
      return;
   }

   pendingDialog = std::make_unique<contomap::frontend::StyleDialog>(inputRequestHandler, layout, style.value());
}

void MainWindow::saveAs(std::string const &filePath)
{
   auto renderTexture = LoadRenderTexture(1920, 1080);

   // TODO: pre-render, determine size, then project

   {
      BeginTextureMode(renderTexture);
      auto renderContext = RenderContext::fromCurrentState();

      drawBackground();
      drawMap(renderContext);
      EndTextureMode();
   }

   auto image = LoadImageFromTexture(renderTexture.texture);
   ImageFlipVertical(&image);
   int fileSize = 0;
   auto exported = ExportImageToMemory(image, ".png", &fileSize);
   if (exported != nullptr)
   {
      SaveFileData(filePath.c_str(), exported, fileSize);
      RL_FREE(exported);

      // TODO: inform display environment of saved file -> download in browser environment
   }
   UnloadImage(image);
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
