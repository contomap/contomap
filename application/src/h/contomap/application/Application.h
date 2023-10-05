#pragma once

#include "contomap/editor/Editor.h"
#include "contomap/frontend/MainWindow.h"

namespace contomap::application
{

/**
 * Application binds all components together. It is the main entry point for the main() functions.
 */
class Application
{
public:
   /**
    * Constructor.
    *
    * @param displayEnvironment the display environment to run in.
    */
   explicit Application(contomap::frontend::DisplayEnvironment &displayEnvironment);

   /**
    * Initialize and open the window to work in.
    */
   void initWindow();

   /**
    * Called if, through external request, the application should shut down.
    */
   void closeRequested();

   /**
    * This method is periodically called to process the next frame: handle state changes and draw the next frame.
    */
   void nextFrame();

   /**
    * Forces a shutdown of the application. This method is called last.
    */
   void close();

private:
   contomap::editor::Editor editor;
   contomap::frontend::MainWindow mainWindow;
};

} // namespace contomap::editor
