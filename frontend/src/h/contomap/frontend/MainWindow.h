#pragma once

#include "contomap/frontend/DisplayEnvironment.h"

namespace contomap::frontend
{

class MainWindow
{
public:
   explicit MainWindow(DisplayEnvironment &environment);

   void closeRequested();
   void drawFrame();

private:
   DisplayEnvironment &environment;
};

} // namespace contomap::frontend
