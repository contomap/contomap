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

   SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);

   auto initialSize = MainWindow::DEFAULT_SIZE;
   InitWindow(initialSize.getWidth().raw<int>(), initialSize.getHeight().raw<int>(), MainWindow::DEFAULT_TITLE);
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
