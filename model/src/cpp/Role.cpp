#include "contomap/model/Role.h"

using contomap::model::Identifier;
using contomap::model::Role;

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
