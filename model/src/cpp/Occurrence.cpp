#include "contomap/model/Occurrence.h"

using contomap::model::Identifier;
using contomap::model::Occurrence;

Occurrence::Occurrence(Identifier id, Identifiers scope, SpacialCoordinate spacial)
   : id(id)
   , scope(std::move(scope))
   , location(spacial)
{
}

contomap::model::Coordinates const &Occurrence::getLocation() const
{
   return location;
}

bool Occurrence::isIn(contomap::model::Identifiers const &thatScope) const
{
   return thatScope.contains(scope);
}
