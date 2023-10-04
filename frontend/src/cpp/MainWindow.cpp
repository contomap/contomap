#include <cmath>

#include <raygui/raygui.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include <raylib.h>
#include <raymath.h>
#pragma GCC diagnostic pop

#include "contomap/frontend/HelpDialog.h"
#include "contomap/frontend/MainWindow.h"
#include "contomap/frontend/NewTopicDialog.h"
#include "contomap/model/Associations.h"
#include "contomap/model/Topics.h"

using contomap::editor::InputRequestHandler;
using contomap::editor::SelectedType;
using contomap::editor::SelectionAction;
using contomap::editor::SelectionMode;
using contomap::frontend::MainWindow;
using contomap::frontend::MapCamera;
using contomap::frontend::RenderContext;
using contomap::model::Association;
using contomap::model::Associations;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::Role;
using contomap::model::SpacialCoordinate;
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

MainWindow::Focus::Focus()
   : distance(std::numeric_limits<float>::max())
{
}

void MainWindow::Focus::registerItem(std::shared_ptr<FocusItem> newItem, float newDistance)
{
   if (newDistance < distance)
   {
      item = std::move(newItem);
      distance = newDistance;
   }
}

void MainWindow::Focus::modifySelection(InputRequestHandler &handler, SelectionAction action, SelectionMode mode) const
{
   if (item != nullptr)
   {
      item->modifySelection(handler, action, mode);
   }
   else
   {
      handler.clearSelection();
   }
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
         mapCamera.panTo(MapCamera::HOME_POSITION);
      }

      bool isInsertOperation = IsKeyPressed(KEY_INSERT) || IsKeyPressed(KEY_I);
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

      if (IsKeyPressed(KEY_L))
      {
         inputRequestHandler.linkSelection();
      }

      if (IsKeyPressed(KEY_DELETE))
      {
         inputRequestHandler.deleteSelection();
      }

      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
      {
         auto action = IsKeyDown(KEY_LEFT_CONTROL) ? SelectionAction::Toggle : SelectionAction::Set;
         auto mode = IsKeyDown(KEY_LEFT_SHIFT) ? SelectionMode::Spread : SelectionMode::Sole;
         currentFocus.modifySelection(inputRequestHandler, action, mode);
      }
   }
}

void MainWindow::updateState()
{
   auto frameTime = contomap::frontend::FrameTime::fromLastFrame();
   mapCamera.timePassed(frameTime);
}

void MainWindow::drawBackground()
{
   ClearBackground(WHITE);
}

void MainWindow::drawMap(RenderContext const &context)
{
   auto contentSize = context.getContentSize();
   auto projection = mapCamera.beginProjection(contentSize);
   auto focusCoordinate = projection.unproject(GetMousePosition());
   Focus focus;

   auto const &viewScope = view.ofViewScope();
   auto const &map = view.ofMap();
   auto const &selection = view.ofSelection();

   // TODO: rework algorithm: need first to determine visible/referenced topics & associations; declutter; draw player lines; draw topics; animate!

   // TODO: find better way to indicate selected and focused items.

   Identifiers associationIds;
   std::map<Identifier, Vector2> associationLocationsById;

   auto visibleAssociations = map.find(Associations::thatAreIn(viewScope));
   for (Association const &visibleAssociation : visibleAssociations)
   {
      std::string nameText(" "); // TODO: resolve name. also: why do triangles overlap if name is empty?

      auto spacialLocation = visibleAssociation.getLocation().getSpacial().getAbsoluteReference();
      Vector2 projectedLocation { .x = spacialLocation.X(), .y = spacialLocation.Y() };

      Font font = GetFontDefault();
      float fontSize = 16.0f;
      float spacing = 1.0f;
      auto textSize = MeasureTextEx(font, nameText.c_str(), fontSize, spacing);
      float plateHeight = textSize.y;

      Color plateBackground { 0x80, 0xE0, 0xB0, 0xC0 };
      float leftCutoff = projectedLocation.x - textSize.x / 2.0f;
      float rightCutoff = projectedLocation.x + textSize.x / 2.0f;

      Rectangle area {
         .x = leftCutoff - plateHeight / 2.0f,
         .y = projectedLocation.y - textSize.y / 2.0f,
         .width = textSize.x + plateHeight,
         .height = plateHeight,
      };

      associationIds.add(visibleAssociation.getId());
      associationLocationsById[visibleAssociation.getId()] = projectedLocation; // TODO: store collision area, for intersection later

      if (CheckCollisionPointRec(focusCoordinate, area))
      {
         focus.registerItem(std::make_shared<AssociationFocusItem>(visibleAssociation.getId()), Vector2Distance(focusCoordinate, projectedLocation));
      }

      if (selection.contains(SelectedType::Association, visibleAssociation.getId()))
      {
         plateBackground = ColorTint(plateBackground, Color { 0xFF, 0xFF, 0xFF, 0x80 });
      }
      if (currentFocus.isAssociation(visibleAssociation.getId()))
      {
         plateBackground = ColorTint(plateBackground, Color { 0xFF, 0xFF, 0xFF, 0x40 });
      }

      DrawTriangle(Vector2 { .x = leftCutoff, .y = projectedLocation.y - plateHeight / 2.0f },
         Vector2 { .x = leftCutoff - plateHeight / 2.0f, .y = projectedLocation.y }, Vector2 { .x = leftCutoff, .y = projectedLocation.y + plateHeight / 2.0f },
         plateBackground);
      DrawRectangleRec(Rectangle { .x = leftCutoff, .y = projectedLocation.y - plateHeight / 2.0f, .width = rightCutoff - leftCutoff, .height = plateHeight },
         plateBackground);
      DrawTriangle(Vector2 { .x = rightCutoff, .y = projectedLocation.y - plateHeight / 2.0f },
         Vector2 { .x = rightCutoff, .y = projectedLocation.y + plateHeight / 2.0f },
         Vector2 { .x = rightCutoff + plateHeight / 2.0f, .y = projectedLocation.y }, plateBackground);

      DrawTextEx(font, nameText.c_str(), Vector2 { .x = projectedLocation.x - textSize.x / 2.0f, .y = projectedLocation.y - textSize.y / 2.0f }, fontSize,
         spacing, Color { 0x00, 0x00, 0x00, 0xFF });
   }

   auto visibleTopics = map.find(Topics::thatAreIn(viewScope));
   for (Topic const &visibleTopic : visibleTopics)
   {
      std::string nameText;
      auto allNames = visibleTopic.allNames();
      for (TopicName const &name : allNames)
      {
         // TODO: filter if in scope, calculate full name plate
         nameText = name.getValue().raw();
      }

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
            auto associationLocation = associationLocationsById[role.getParent()];

            if (CheckCollisionPointLine(focusCoordinate, projectedLocation, associationLocation, 5))
            {
               focus.registerItem(std::make_shared<RoleFocusItem>(role.getId()), 0.0f);
            }

            Color roleColor { 0x00, 0x00, 0x00, 0xFF };
            float thickness = 1.0f;
            if (selection.contains(SelectedType::Role, role.getId()))
            {
               roleColor = ColorTint(roleColor, Color { 0xFF, 0x00, 0x00, 0x80 });
               thickness += 2.0f;
            }
            if (currentFocus.isRole(role.getId()))
            {
               roleColor = ColorTint(roleColor, Color { 0xFF, 0xFF, 0xFF, 0x40 });
               thickness += 0.5f;
            }

            DrawLineEx(projectedLocation, associationLocation, thickness, roleColor);
         }

         Font font = GetFontDefault();
         float fontSize = 16.0f;
         float spacing = 1.0f;
         auto textSize = MeasureTextEx(font, nameText.c_str(), fontSize, spacing);
         float plateHeight = textSize.y;

         Color plateBackground { 0xB0, 0x80, 0xE0, 0xC0 };
         float leftCutoff = projectedLocation.x - textSize.x / 2.0f;
         float rightCutoff = projectedLocation.x + textSize.x / 2.0f;

         Rectangle area {
            .x = leftCutoff - plateHeight / 2.0f,
            .y = projectedLocation.y - textSize.y / 2.0f,
            .width = textSize.x + plateHeight,
            .height = plateHeight,
         };
         if (CheckCollisionPointRec(focusCoordinate, area))
         {
            focus.registerItem(std::make_shared<OccurrenceFocusItem>(occurrence.getId()), Vector2Distance(focusCoordinate, projectedLocation));
         }

         if (selection.contains(SelectedType::Occurrence, occurrence.getId()))
         {
            plateBackground = ColorTint(plateBackground, Color { 0xFF, 0xFF, 0xFF, 0x80 });
         }
         if (currentFocus.isOccurrence(occurrence.getId()))
         {
            plateBackground = ColorTint(plateBackground, Color { 0xFF, 0xFF, 0xFF, 0x40 });
         }

         DrawCircleSector(Vector2 { .x = leftCutoff, .y = projectedLocation.y }, plateHeight / 2.0f, 180.0f, 360.0f, 20, plateBackground);
         DrawRectangleRec(
            Rectangle { .x = leftCutoff, .y = projectedLocation.y - plateHeight / 2.0f, .width = rightCutoff - leftCutoff, .height = plateHeight },
            plateBackground);
         DrawCircleSector(Vector2 { .x = rightCutoff, .y = projectedLocation.y }, plateHeight / 2.0f, 0.0f, 180.0f, 20, plateBackground);

         DrawTextEx(font, nameText.c_str(), Vector2 { .x = projectedLocation.x - textSize.x / 2.0f, .y = projectedLocation.y - textSize.y / 2.0f }, fontSize,
            spacing, Color { 0x00, 0x00, 0x00, 0xFF });
      }
   }

   currentFocus = focus;
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
   Vector2 toolbarPosition { .x = 0, .y = 0 };
   Vector2 toolbarSize { .x = contentSize.x, .y = iconSize + (padding * 2.0f) };
   GuiPanel(Rectangle { toolbarPosition.x, toolbarPosition.y, toolbarSize.x, toolbarSize.y }, nullptr);
   GuiSetTooltip("Show help window");
   if (GuiButton(Rectangle { toolbarPosition.x + toolbarSize.x - (padding + iconSize), toolbarPosition.y + padding, iconSize, iconSize },
          GuiIconText(ICON_HELP, nullptr)))
   {
      openHelpDialog();
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
