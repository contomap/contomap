#pragma once

#include "contomap/model/SpacialCoordinate.h"
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
    * Called when a new topic shall be created.
    *
    * @param name how the new topic shall be named.
    * @param location the point at which the topic shall be put at.
    */
   virtual void newTopicRequested(contomap::model::TopicNameValue name, contomap::model::SpacialCoordinate location) = 0;

   /**
    * Called when a new association shall be created.
    *
    * @param location the point at which the association shall be put at.
    */
   virtual void newAssociationRequested(contomap::model::SpacialCoordinate location) = 0;
};

} // namespace contomap::editor
