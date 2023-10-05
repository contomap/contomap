#include "contomap/application/Application.h"

using contomap::application::Application;

Application::Application(contomap::frontend::DisplayEnvironment &displayEnvironment)
   : mainWindow(displayEnvironment, editor, editor)
{
}

void Application::initWindow()
{
   mainWindow.init();
}

void Application::closeRequested()
{
   mainWindow.closeRequested();
}

void Application::nextFrame()
{
   mainWindow.nextFrame();
}

void Application::close()
{
   mainWindow.close();
}
