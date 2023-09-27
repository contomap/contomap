#include "contomap/model/Role.h"

using contomap::model::Identifier;
using contomap::model::Role;

Role::Role(Identifier id, Identifier parent)
   : id(id)
   , parent(parent)
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
