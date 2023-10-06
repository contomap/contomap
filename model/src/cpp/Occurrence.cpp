#include "contomap/model/Occurrence.h"

using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;

Occurrence::Occurrence(Identifier id, Identifiers scope, SpacialCoordinate spacial)
   : id(id)
   , scope(std::move(scope))
   , location(spacial)
{
}

Identifier Occurrence::getId() const
{
   return id;
}

contomap::model::Coordinates const &Occurrence::getLocation() const
{
   return location;
}

bool Occurrence::isIn(Identifiers const &thatScope) const
{
   return thatScope.contains(scope);
}

bool Occurrence::scopeContains(contomap::model::Identifier thatId) const
{
   return scope.contains(thatId);
}
