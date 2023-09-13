#pragma once

#include <array>

#include "contomap/editor/ViewModel.h"
#include "contomap/model/TopicNameValue.h"

namespace contomap::frontend
{

/**
 * ViewModelState is the ViewModel implementation that supports the MainWindow.
 */
class ViewModelState : public contomap::editor::ViewModel
{
public:
   ViewModelState()
   {
      clearNewTopicName();
   }

   void showNewTopicWindow() override
   {
      newTopicWindowShown = true;
   }
   void hideNewTopicWindow() override
   {
      clearNewTopicName();
      newTopicWindowShown = false;
   }

   /**
    * @return true if currently any window (popup) is visible.
    */
   [[nodiscard]] bool anyWindowShown() const
   {
      return newTopicWindowShown;
   }

   /**
    * This field is true in case a new topic shall be created.
    */
   bool newTopicWindowShown = false;
   /**
    * The current name of the topic to be created.
    */
   std::array<char, contomap::model::TopicNameValue::maxUtf8Bytes() + 1> newTopicName {};

private:
   void clearNewTopicName()
   {
      newTopicName.fill(0x00);
   }
};

} // namespace contomap::frontend
