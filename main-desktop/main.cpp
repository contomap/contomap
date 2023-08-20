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

int main()
{
   DesktopEnvironment environment;
   MainWindow mainWindow(environment);

   auto initialSize = MainWindow::DEFAULT_SIZE;
   InitWindow(static_cast<int>(initialSize.getWidth()), static_cast<int>(initialSize.getHeight()), MainWindow::DEFAULT_TITLE);
   SetTargetFPS(60);
   SetExitKey(KEY_NULL); // Disable default behavior that requests to close window on Escape key.

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
