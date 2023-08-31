
#include <sstream>

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

MainWindow::Size const MainWindow::DEFAULT_SIZE = MainWindow::Size::ofPixel(1600, 900);
char const MainWindow::DEFAULT_TITLE[] = "contomap";

MainWindow::MainWindow(DisplayEnvironment &environment)
   : environment(environment)
{
}

void MainWindow::closeRequested()
{
   environment.closeWindow();
}

void MainWindow::drawFrame()
{
   BeginDrawing();
   ClearBackground(RAYWHITE);

   std::ostringstream text;
   text << "Window is rendered";
   DrawText(text.str().c_str(), 190, 200, 20, LIGHTGRAY);
   EndDrawing();
}
