#pragma once

#include "contomap/editor/ViewModel.h"

namespace contomap::frontend
{

/**
 * ViewModelState is the ViewModel implementation that supports the MainWindow.
 */
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

   /**
    * @return true if currently any window (popup) is visible.
    */
   [[nodiscard]] bool anyWindowShown() const
   {
      return helpWindowShown;
   }

   /**
    * This field is true in case the help window shall be visible.
    */
   bool helpWindowShown = false;
};

} // namespace contomap::frontend
