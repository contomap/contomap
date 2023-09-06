#pragma once

namespace contomap::editor
{

/**
 * ViewModel is the outgoing interface from the editor, towards the frontend.
 */
class ViewModel
{
public:
   virtual ~ViewModel() = default;

   /**
    * The help window shall be shown from now on.
    */
   virtual void showHelpWindow() = 0;

   /**
    * The help window shall be hidden from now on.
    */
   virtual void hideHelpWindow() = 0;

   /**
    * The window to create a new topic shall be shown from now on.
    */
   virtual void showNewTopicWindow() = 0;

   /**
    * The window to create a new topic shall be hidden from now on.
    */
   virtual void hideNewTopicWindow() = 0;
};

} // namespace contomap::editor
