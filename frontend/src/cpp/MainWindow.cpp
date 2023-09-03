
#include <sstream>

#define RAYGUI_IMPLEMENTATION
#include <raygui/raygui.h>
#include <raylib.h>

#include "contomap/frontend/MainWindow.h"

using contomap::frontend::MainWindow;

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

MainWindow::Size const MainWindow::DEFAULT_SIZE = MainWindow::Size::ofPixel(800, 450);
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

   drawBackground();
   drawMap();
   drawUserInterface();

   EndDrawing();
}

void MainWindow::drawBackground()
{
   ClearBackground(WHITE);
}

void MainWindow::drawMap()
{
}

void MainWindow::drawUserInterface()
{
   GuiEnableTooltip();

   auto dpiScale = GetWindowScaleDPI();
   Vector2 contentSize { static_cast<float>(GetRenderWidth()) / dpiScale.x, static_cast<float>(GetRenderHeight()) / dpiScale.y };
   auto buttonHeight = 24.0f;
   auto padding = 2.0f;
   GuiPanel(Rectangle { 0, 0, contentSize.x, buttonHeight + (padding * 2.0f) }, nullptr);
   GuiSetTooltip("Show help window");
   if (GuiButton(Rectangle { contentSize.x - (padding + buttonHeight) * 1, padding, buttonHeight, buttonHeight }, GuiIconText(ICON_HELP, nullptr)))
   {
      inputRequestHandler.helpWindowRequested();
   }
}
