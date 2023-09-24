#include "contomap/model/Association.h"

using contomap::model::Association;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::SpacialCoordinate;

Association::Association(Identifier id, Identifiers scope, SpacialCoordinate spacial)
   : id(id)
   , scope(std::move(scope))
   , location(spacial)
{
}

Identifier Association::getId() const
{
   return id;
}

contomap::model::Coordinates const &Association::getLocation() const
{
   return location;
}

bool Association::isIn(Identifiers const &thatScope) const
{
   return thatScope.contains(scope);
}