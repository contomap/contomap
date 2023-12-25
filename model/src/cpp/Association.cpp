#include "contomap/model/Association.h"
#include "contomap/model/Role.h"
#include "contomap/model/Topic.h"

using contomap::infrastructure::Link;
using contomap::infrastructure::Links;
using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Association;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Role;
using contomap::model::SpacialCoordinate;
using contomap::model::Style;
using contomap::model::Topic;

Association::Association(Identifier id)
   : id(id)
{
}

Association::Association(Identifier id, Identifiers scope, SpacialCoordinate spacial)
   : Scoped(std::move(scope))
   , id(id)
   , location(spacial)
{
}

void Association::encodeProperties(Encoder &coder) const
{
   Coder::Scope propertiesScope(coder, "properties");
   encodeScoped(coder);
   location.encode(coder, "location");
   encodeTypeable(coder);
   appearance.encode(coder, "appearance");
   encodeReifiable(coder);
}

void Association::decodeProperties(contomap::infrastructure::serial::Decoder &coder, uint8_t version, std::function<Topic &(Identifier)> const &topicResolver)
{
   Coder::Scope propertiesScope(coder, "properties");
   decodeScoped(coder, topicResolver);
   location.decode(coder, "location", version);
   decodeTypeable(coder, topicResolver);
   appearance.decode(coder, "appearance", version);
   decodeReifiable(coder, topicResolver);
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

void Association::moveBy(SpacialCoordinate::Offset offset)
{
   location.moveBy(offset);
}

std::unique_ptr<Link<Association>> Association::link(Role &role, std::function<void()> associationUnlinked)
{
   Identifier roleId = role.getId();
   auto links = Links::between(*this, std::move(associationUnlinked), role, [this, roleId]() { roles.erase(roleId); });
   roles.emplace(roleId, std::make_unique<RoleEntry>(std::move(links.second)));
   return std::move(links.first);
}

bool Association::hasRoles() const
{
   return !roles.empty();
}

void Association::setAppearance(Style style)
{
   appearance = std::move(style);
}

Style Association::getAppearance() const
{
   return appearance;
}
