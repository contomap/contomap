#include "contomap/model/Association.h"
#include "contomap/model/Role.h"

using contomap::model::Association;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Role;
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

void Association::moveTo(SpacialCoordinate absolutePosition)
{
   location.setSpacial(absolutePosition);
}

bool Association::isIn(Identifiers const &thatScope) const
{
   return thatScope.contains(scope);
}

void Association::addRole(Role const &role)
{
   if (role.getParent() != id)
   {
      // TODO: consider throwing an exception
      return;
   }
   roles.add(role.getId());
}

bool Association::removeRole(Role const &role)
{
   return roles.remove(role.getId());
}

Identifiers Association::allRoles() const
{
   return roles;
}
