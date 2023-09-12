#pragma once

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/editor/ViewModel.h"

namespace contomap::editor
{

/**
 * Editor is the core domain of the application. It contains the logic, regardless of external interfaces.
 */
class Editor : public contomap::editor::InputRequestHandler
{
public:
   /**
    * This method must be called during initialization in order to register the
    * ViewModel that shall be used for updating the state.
    *
    * @param viewModel the view model that receives all updates from now on.
    */
   void use(contomap::editor::ViewModel &viewModel);

   void helpWindowShowRequested() override;
   void helpWindowHideRequested() override;

   void newTopicRequested() override;
   void newTopicRequestAborted() override;
   void newTopicRequested(contomap::model::TopicNameValue name) override;

private:
   contomap::editor::ViewModel &viewModel();

   contomap::editor::ViewModel *currentViewModel = nullptr;
};

} // namespace contomap::editor
