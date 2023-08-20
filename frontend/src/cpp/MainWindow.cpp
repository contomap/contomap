
#include <sstream>

#include <raylib.h>

#include "contomap/frontend/MainWindow.h"

using contomap::frontend::MainWindow;

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
   text << "Congrats! You created your first window!";
   DrawText(text.str().c_str(), 190, 200, 20, LIGHTGRAY);
   EndDrawing();
}
