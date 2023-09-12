#include "contomap/model/Role.h"

using contomap::model::Identifier;
using contomap::model::Role;

Role::Role(Identifier id, Identifier parent)
   : id(id)
   , parent(parent)
{
}
