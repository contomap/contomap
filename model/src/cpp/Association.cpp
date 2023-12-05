#include "contomap/model/Association.h"
#include "contomap/model/Role.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Association;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::OptionalIdentifier;
using contomap::model::Role;
using contomap::model::SpacialCoordinate;
using contomap::model::Style;

Association::Association(Identifier id)
   : id(id)
{
}

Association::Association(Identifier id, Identifiers scope, SpacialCoordinate spacial)
   : id(id)
   , scope(std::move(scope))
   , location(spacial)
{
}

void Association::encodeProperties(Encoder &coder) const
{
   Coder::Scope propertiesScope(coder, "properties");
   scope.encode(coder, "scope");
   type.encode(coder, "type");
   appearance.encode(coder, "appearance");
   location.encode(coder, "location");
}

void Association::decodeProperties(contomap::infrastructure::serial::Decoder &coder, uint8_t version)
{
   Coder::Scope propertiesScope(coder, "properties");
   scope.decode(coder, "scope");
   type = OptionalIdentifier::from(coder, "type");
   appearance.decode(coder, "appearance", version);
   location.decode(coder, "location", version);
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

bool Association::isWithoutScope() const
{
   return scope.empty();
}

Role::Seed Association::addRole()
{
   auto roleId = Identifier::random();
   roles.add(roleId);
   return { roleId, getId() };
}

bool Association::removeRole(Role const &role)
{
   return roles.remove(role.getId());
}

bool Association::hasRole(Identifier roleId) const
{
   return roles.contains(roleId);
}

bool Association::hasRoles() const
{
   return !roles.empty();
}

void Association::removeTopicReferences(Identifier topicId)
{
   if (scope.contains(topicId))
   {
      scope.clear();
   }
   if (type.isAssigned() && (type.value() == topicId))
   {
      type.clear();
   }
}

void Association::setAppearance(Style style)
{
   appearance = std::move(style);
}

Style Association::getAppearance() const
{
   return appearance;
}

void Association::setType(Identifier typeTopicId)
{
   type = typeTopicId;
}

void Association::clearType()
{
   type.clear();
}

OptionalIdentifier Association::getType() const
{
   return type;
}
