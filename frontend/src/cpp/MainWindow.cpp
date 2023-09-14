#include <raygui/raygui.h>
#include <raylib.h>

#include "contomap/frontend/HelpDialog.h"
#include "contomap/frontend/MainWindow.h"
#include "contomap/frontend/NewTopicDialog.h"

using contomap::frontend::MainWindow;
using contomap::frontend::RenderContext;
using contomap::model::TopicNameValue;

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

MainWindow::MainWindow(DisplayEnvironment &environment, contomap::editor::InputRequestHandler &inputRequestHandler)
   : environment(environment)
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
   Camera2D cam;

   auto contentSize = context.getContentSize();

   cam.offset = Vector2 { contentSize.x / 2.0f, contentSize.y / 2.0f };
   cam.target = Vector2 { 0.0f, 0.0f };
   cam.rotation = 0.0f;
   cam.zoom = 10.0f;
   BeginMode2D(cam);

   /*
      DrawCircleSector(Vector2 { -15.0f, 0.0f }, 20.0f, 180.0f, 360.0f, 20, Color { 0xFF, 0x00, 0x00, 0x80 });
      DrawRectangle(-15, -20, 30, 40, Color { 0x00, 0xFF, 0x00, 0x80 });
      DrawCircleSector(Vector2 { 15.0f, 0.0f }, 20.0f, 0.0f, 180.0f, 20, Color { 0x00, 0x00, 0xFF, 0x80 });

      Font font = GetFontDefault();
      std::string text("test");
      float fontSize = 30.0f;
      float spacing = 0.0f;
      auto textSize = MeasureTextEx(font, text.c_str(), fontSize, spacing);
      DrawTextEx(font, text.c_str(), Vector2 { -textSize.x / 2.0f, -textSize.y / 2.0f }, fontSize, spacing, Color { 0x00, 0x00, 0x00, 0xFF });
   */
   EndMode2D();
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

      if (IsKeyPressed(KEY_INSERT))
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
