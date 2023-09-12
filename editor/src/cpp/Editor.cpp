#include <stdexcept>

#include "contomap/editor/Editor.h"

using contomap::editor::Editor;
using contomap::model::TopicNameValue;

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

void Editor::newTopicRequested()
{
   // TODO: should all previous windows be closed, or should rather be a state-machine be kept, that considers/ignores requests?
   viewModel().showNewTopicWindow();
}

void Editor::newTopicRequestAborted()
{
   viewModel().hideNewTopicWindow();
}

void Editor::newTopicRequested(TopicNameValue name)
{
   viewModel().hideNewTopicWindow();
}

contomap::editor::ViewModel &Editor::viewModel()
{
   if (currentViewModel == nullptr)
   {
      throw std::runtime_error("ViewModel is not set");
   }
   return *currentViewModel;
}
