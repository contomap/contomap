#pragma once

#include "contomap/model/Coordinates.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/OptionalIdentifier.h"

namespace contomap::model
{

/**
 * An Association represents the link or relation between topics.
 */
class Association
{
public:
   /**
    * Constructor.
    *
    * @param id the primary identifier of this association.
    */
   explicit Association(contomap::model::Identifier id);

private:
   contomap::model::Identifier id;
   contomap::model::Identifiers scope;

   contomap::model::OptionalIdentifier reifier;
   contomap::model::OptionalIdentifier type;

   contomap::model::OptionalIdentifier appearance;
   contomap::model::Coordinates location;

   contomap::model::Identifiers roles;
};

}
