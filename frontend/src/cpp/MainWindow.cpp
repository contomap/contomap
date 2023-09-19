#include <raygui/raygui.h>
#include <raylib.h>

#include "contomap/frontend/HelpDialog.h"
#include "contomap/frontend/MainWindow.h"
#include "contomap/frontend/NewTopicDialog.h"
#include "contomap/model/Topics.h"

using contomap::frontend::MainWindow;
using contomap::frontend::RenderContext;
using contomap::model::Occurrence;
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

MainWindow::MainWindow(DisplayEnvironment &environment, contomap::editor::View &view, contomap::editor::InputRequestHandler &inputRequestHandler)
   : environment(environment)
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

void MainWindow::drawFrame()
{
   BeginDrawing();

   auto renderContext = RenderContext::fromCurrentState();

   drawBackground();
   drawMap(renderContext);
   drawUserInterface(renderContext);

   EndDrawing();
}

void MainWindow::drawBackground()
{
   ClearBackground(WHITE);
}

void MainWindow::drawMap(RenderContext const &context)
{
   auto contentSize = context.getContentSize();
   auto projection = mapCamera.beginProjection(contentSize);

   auto const &viewScope = view.ofViewScope();
   auto const &map = view.ofMap();

   auto visibleTopics = map.findTopics(Topics::thatAreIn(viewScope));
   // TODO: rework algorithm: need first to determine visible/referenced topics; declutter; draw associations; draw topics; animate!
   for (Topic const &visibleTopic : visibleTopics)
   {
      std::string nameText;
      auto allNames = visibleTopic.allNames();
      for (TopicName const &name : allNames)
      {
         // TODO: filter if in scope, calculate full name plate
         nameText = name.getValue().raw();
      }

      for (Occurrence const &occurrence : visibleTopic.occurrencesIn(viewScope))
      {
         auto spacialLocation = occurrence.getLocation().getSpacial().getAbsoluteReference();
         Vector2 projectedLocation { .x = spacialLocation.X(), .y = spacialLocation.Y() };

         Font font = GetFontDefault();
         float fontSize = 16.0f;
         float spacing = 1.0f;
         auto textSize = MeasureTextEx(font, nameText.c_str(), fontSize, spacing);
         float plateHeight = textSize.y;

         Color plateBackground { 0xB0, 0x80, 0xE0, 0xC0 };
         float leftCutoff = projectedLocation.x - textSize.x / 2.0f;
         float rightCutoff = projectedLocation.x + textSize.x / 2.0f;
         DrawCircleSector(Vector2 { .x = leftCutoff, .y = projectedLocation.y }, plateHeight / 2.0f, 180.0f, 360.0f, 20, plateBackground);
         DrawRectangleRec(
            Rectangle { .x = leftCutoff, .y = projectedLocation.y - plateHeight / 2.0f, .width = rightCutoff - leftCutoff, .height = plateHeight },
            plateBackground);
         DrawCircleSector(Vector2 { .x = rightCutoff, .y = projectedLocation.y }, plateHeight / 2.0f, 0.0f, 180.0f, 20, plateBackground);

         DrawTextEx(font, nameText.c_str(), Vector2 { .x = projectedLocation.x - textSize.x / 2.0f, .y = projectedLocation.y - textSize.y / 2.0f }, fontSize,
            spacing, Color { 0x00, 0x00, 0x00, 0xFF });
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

      // TODO: shortcut system
      if (IsKeyPressed(KEY_INSERT) || (IsKeyPressed(KEY_I) && IsKeyDown(KEY_LEFT_CONTROL)))
      {
         openNewTopicDialog();
      }
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
   pendingDialog = std::make_unique<contomap::frontend::NewTopicDialog>(inputRequestHandler, layout);
}
