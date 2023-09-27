#include "contomap/editor/Selection.h"

using contomap::editor::SelectedType;
using contomap::editor::Selection;
using contomap::model::Identifier;

void Selection::clear()
{
   identifiers.clear();
}

void Selection::setSole(SelectedType type, Identifier id)
{
   clear();
   identifiers[type].add(id);
}

void Selection::toggle(SelectedType type, Identifier id)
{
   auto &specific = identifiers[type];
   if (specific.contains(id))
   {
      specific.remove(id);
   }
   else
   {
      specific.add(id);
   }
}

bool Selection::contains(SelectedType type, Identifier id) const
{
   auto it = identifiers.find(type);
   return (it != identifiers.end()) && (it->second.contains(id));
}
