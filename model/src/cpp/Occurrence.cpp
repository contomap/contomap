#include "contomap/model/Occurrence.h"

using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;

Occurrence::Occurrence(Identifier id, Identifier topicId, Identifiers scope, SpacialCoordinate spacial)
   : id(id)
   , topicId(topicId)
   , scope(std::move(scope))
   , location(spacial)
{
}

Identifier Occurrence::getId() const
{
   return id;
}

Identifier Occurrence::getTopicId() const
{
   return topicId;
}

Identifiers const &Occurrence::getScope() const
{
   return scope;
}

contomap::model::Coordinates const &Occurrence::getLocation() const
{
   return location;
}

bool Occurrence::isIn(Identifiers const &thatScope) const
{
   return thatScope.contains(scope);
}

bool Occurrence::scopeContains(Identifier thatId) const
{
   return scope.contains(thatId);
}
