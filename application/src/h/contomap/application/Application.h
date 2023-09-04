#pragma once

#include "contomap/editor/Editor.h"
#include "contomap/frontend/MainWindow.h"

namespace contomap::application
{

class Application
{
public:
   explicit Application(contomap::frontend::DisplayEnvironment &displayEnvironment);

   void initWindow();
   void closeRequested();
   void drawFrame();
   void close();

private:
   contomap::editor::Editor editor;
   contomap::frontend::MainWindow mainWindow;
};

} // namespace contomap::editor
