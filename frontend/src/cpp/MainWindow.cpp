
#include <sstream>

#include <raylib.h>

#include "contomap/frontend/MainWindow.h"

using contomap::frontend::MainWindow;

MainWindow::Size::Size(uint32_t width, uint32_t height)
   : width(width)
   , height(height)
{
}

uint32_t MainWindow::Size::getWidth() const
{
   return width;
}

uint32_t MainWindow::Size::getHeight() const
{
   return height;
}

MainWindow::Size const MainWindow::DEFAULT_SIZE(1600, 900);
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
