#pragma once

namespace contomap::editor
{

/**
 * InputRequestHandler is the incoming interface from user requests.
 */
class InputRequestHandler
{
public:
   virtual ~InputRequestHandler() = default;

   /**
    * Called when the help window shall be shown.
    */
   virtual void helpWindowShowRequested() = 0;

   /**
    * Called when the help window shall be hidden.
    */
   virtual void helpWindowHideRequested() = 0;
};

} // namespace contomap::editor
