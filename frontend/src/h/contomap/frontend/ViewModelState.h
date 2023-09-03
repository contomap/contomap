#pragma once

#include "contomap/editor/ViewModel.h"

namespace contomap::frontend
{

class ViewModelState : public contomap::editor::ViewModel
{
public:
   void showHelpWindow() override
   {
      helpWindowShown = true;
   }
   void hideHelpWindow() override
   {
      helpWindowShown = false;
   }

   [[nodiscard]] bool anyWindowShown() const
   {
      return helpWindowShown;
   }

   bool helpWindowShown = false;
};

} // namespace contomap::frontend
