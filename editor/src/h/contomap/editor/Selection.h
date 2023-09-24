#pragma once

#include "contomap/model/Identifiers.h"

namespace contomap::editor
{

/**
 * Selection keeps track of items that are currently highlighted.
 */
class Selection
{
public:
   /**
    * Default constructor.
    */
   Selection() = default;

   /**
    * Empties the selection.
    */
   void clear();

   /**
    * Set the selection to be for the identified topic.
    *
    * @param id identifier of the topic.
    */
   void setForTopic(contomap::model::Identifier id);

   /**
    * Set the selection to be for the identified association.
    *
    * @param id identifier of the association.
    */
   void setForAssociation(contomap::model::Identifier id);

private:
   contomap::model::Identifiers topics;
   contomap::model::Identifiers associations;
};

} // namespace contomap::frontend
