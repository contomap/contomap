#include <raylib.h>

#include "contomap/frontend/MainWindow.h"

using contomap::frontend::DisplayEnvironment;
using contomap::frontend::MainWindow;

class DesktopEnvironment : public DisplayEnvironment
{
public:
   void closeWindow() override
   {
      shouldClose = true;
   }

   bool isFullscreen() override
   {
      return IsWindowFullscreen();
   }

   void toggleFullscreen() override
   {
      ToggleFullscreen();
   }

   [[nodiscard]] bool shouldCloseWindow() const
   {
      return shouldClose;
   }

private:
   bool shouldClose = false;
};

int screenWidth = 800;
int screenHeight = 450;

int main()
{
   DesktopEnvironment environment;
   MainWindow mainWindow(environment);

   InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
   SetTargetFPS(60);
   SetExitKey(KEY_NULL);

   while (!environment.shouldCloseWindow())
   {
      if (WindowShouldClose())
      {
         mainWindow.closeRequested();
      }
      mainWindow.drawFrame();
   }

   CloseWindow();
   return 0;
}
