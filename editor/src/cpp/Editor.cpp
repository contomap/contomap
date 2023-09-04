#include "contomap/editor/Editor.h"

using contomap::editor::Editor;

void Editor::use(contomap::editor::ViewModel &viewModel)
{
   currentViewModel = &viewModel;
}

void Editor::helpWindowShowRequested()
{
   viewModel().showHelpWindow();
}

void Editor::helpWindowHideRequested()
{
   viewModel().hideHelpWindow();
}

contomap::editor::ViewModel &Editor::viewModel()
{
   if (currentViewModel == nullptr)
   {
      throw "ViewModel is not set";
   }
   return *currentViewModel;
}
