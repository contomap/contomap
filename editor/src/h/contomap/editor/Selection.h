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
    * Set the selection to be for the identified occurrence.
    *
    * @param id identifier of the occurrence.
    */
   void setForOccurrence(contomap::model::Identifier id);

   /**
    * Toggles the selection of the identified occurrence.
    *
    * @param id identifier for the occurrence.
    */
   void toggleOccurrence(contomap::model::Identifier id);

   /**
    * @param id identifier for the occurrence to check.
    * @return true if the identified occurrence is in the selection.
    */
   [[nodiscard]] bool containsOccurrence(contomap::model::Identifier id) const;

   /**
    * Set the selection to be for the identified association.
    *
    * @param id identifier of the association.
    */
   void setForAssociation(contomap::model::Identifier id);

   /**
    * Toggles the selection of the identified association.
    *
    * @param id identifier for the association.
    */
   void toggleAssociation(contomap::model::Identifier id);

   /**
    * @param id identifier for the association to check.
    * @return true if the identified association is in the selection.
    */
   bool containsAssociation(contomap::model::Identifier id) const;

private:
   contomap::model::Identifiers occurrences;
   contomap::model::Identifiers associations;
};

} // namespace contomap::frontend
