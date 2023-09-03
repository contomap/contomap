#pragma once

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/editor/ViewModel.h"

namespace contomap::editor
{

class Editor : public contomap::editor::InputRequestHandler
{
public:
   void use(contomap::editor::ViewModel &viewModel);

   void helpWindowShowRequested() override;
   void helpWindowHideRequested() override;

private:
   contomap::editor::ViewModel &viewModel();

   contomap::editor::ViewModel *currentViewModel = nullptr;
};

} // namespace contomap::editor
