#pragma once

#include "contomap/model/TopicNameValue.h"

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

   /**
    * Called when a new topic is requested.
    */
   virtual void newTopicRequested() = 0;

   /**
    * Called when the recent request to create a new topic was aborted.
    */
   virtual void newTopicRequestAborted() = 0;

   /**
    * Called when a new topic shall be created.
    *
    * @param name how the new topic shall be named
    */
   virtual void newTopicRequested(contomap::model::TopicNameValue name) = 0;
};

} // namespace contomap::editor
