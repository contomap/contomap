#include "contomap/model/Occurrence.h"

using contomap::model::Identifier;
using contomap::model::Occurrence;

Occurrence::Occurrence(Identifier id, Identifiers scope, SpacialCoordinate spacial)
   : id(id)
   , scope(std::move(scope))
   , location(spacial)
{
}
