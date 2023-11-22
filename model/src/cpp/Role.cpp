#include "contomap/model/Role.h"

using contomap::model::Identifier;
using contomap::model::OptionalIdentifier;
using contomap::model::Role;
using contomap::model::Style;

Role::Role(Seed const &seed)
   : id(seed.id)
   , parent(seed.parent)
{
}

Identifier Role::getId() const
{
   return id;
}

Identifier Role::getParent() const
{
   return parent;
}

void Role::setAppearance(Style style)
{
   appearance = std::move(style);
}

Style Role::getAppearance() const
{
   return appearance;
}

void Role::setType(Identifier typeTopicId)
{
   type = typeTopicId;
}

void Role::clearType()
{
   type.clear();
}

OptionalIdentifier Role::getType() const
{
   return type;
}
