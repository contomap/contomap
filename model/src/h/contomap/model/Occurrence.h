#pragma once

#include "contomap/model/Coordinates.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/OptionalIdentifier.h"

namespace contomap::model
{

/**
 * An Occurrence represents the presence of a topic.
 */
class Occurrence
{
public:
   /**
    * Constructor.
    *
    * @param id the primary identifier of this occurrence.
    * @param scope the scope within which this occurrence is valid.
    * @param spacial the known, initial point where the occurrence is happening.
    */
   explicit Occurrence(contomap::model::Identifier id, contomap::model::Identifiers scope, contomap::model::SpacialCoordinate spacial);

   /**
    * @return the unique identifier of this occurrence instance.
    */
   [[nodiscard]] contomap::model::Identifier getId() const;

   /**
    * @return the location of this occurrence
    */
   [[nodiscard]] contomap::model::Coordinates const &getLocation() const;

   /**
    * Return true if this instance is in the given scope.
    *
    * @param thatScope the scope to look for.
    * @return true if the occurrence is in given scope.
    */
   [[nodiscard]] bool isIn(contomap::model::Identifiers const &thatScope) const;

   /**
    * Return true if the scope contains the given identifier.
    *
    * @param thatId the identifier to check .
    * @return true in case the identifier is part of the scope.
    */
   [[nodiscard]] bool scopeContains(contomap::model::Identifier thatId) const;

private:
   contomap::model::Identifier id;
   contomap::model::Identifiers scope;

   contomap::model::OptionalIdentifier reifier;
   contomap::model::OptionalIdentifier type;

   contomap::model::OptionalIdentifier appearance;
   contomap::model::Coordinates location;
};

}
