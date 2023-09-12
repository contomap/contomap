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
    */
   explicit Occurrence(contomap::model::Identifier id);

private:
   contomap::model::Identifier id;
   contomap::model::Identifiers scope;

   contomap::model::OptionalIdentifier reifier;
   contomap::model::OptionalIdentifier type;

   contomap::model::OptionalIdentifier appearance;
   contomap::model::Coordinates location;
};

}
