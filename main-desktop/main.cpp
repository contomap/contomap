#include <raylib.h>

#include "contomap/application/Application.h"

using contomap::application::Application;
using contomap::frontend::DisplayEnvironment;

/**
 * DesktopEnvironment represents the display environment for a standalone application.
 */
class DesktopEnvironment : public DisplayEnvironment
{
public:
   void closeWindow() override
   {
      shouldClose = true;
   }

   /**
    * @return true in case the window should be closed.
    */
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
   Application app(environment);

   SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);

   app.initWindow();
   SetTargetFPS(60);
   SetExitKey(KEY_NULL); // Disable default behavior that requests to close window on Escape key.

   while (!environment.shouldCloseWindow())
   {
      if (WindowShouldClose())
      {
         app.closeRequested();
      }
      app.drawFrame();
   }
   app.close();
   return 0;
}
