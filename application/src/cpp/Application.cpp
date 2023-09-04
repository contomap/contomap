#include "contomap/application/Application.h"

using contomap::application::Application;

Application::Application(contomap::frontend::DisplayEnvironment &displayEnvironment)
   : mainWindow(displayEnvironment, editor)
{
   editor.use(mainWindow.viewModel());
}

void Application::initWindow()
{
   mainWindow.init();
}

void Application::closeRequested()
{
   mainWindow.closeRequested();
}

void Application::drawFrame()
{
   mainWindow.drawFrame();
}

void Application::close()
{
   mainWindow.close();
}
