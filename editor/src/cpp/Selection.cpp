#include <algorithm>

#include "contomap/editor/Selection.h"

using contomap::editor::SelectedType;
using contomap::editor::Selection;
using contomap::model::Identifier;
using contomap::model::Identifiers;

bool Selection::empty() const
{
   return std::all_of(identifiers.begin(), identifiers.end(), [](std::map<SelectedType, Identifiers>::value_type const &kvp) { return kvp.second.empty(); });
}

void Selection::clear()
{
   identifiers.clear();
}

bool Selection::hasSoleEntryFor(SelectedType type) const
{
   return identifiers.contains(type) && std::all_of(identifiers.begin(), identifiers.end(), [type](std::map<SelectedType, Identifiers>::value_type const &kvp) {
      auto const &[otherType, specific] = kvp;
      return ((otherType == type) && (specific.size() == 1)) || ((otherType != type) && specific.empty());
   });
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

Identifiers const &Selection::of(SelectedType type) const
{
   static Identifiers const empty;
   auto it = identifiers.find(type);
   return (it != identifiers.end()) ? it->second : empty;
}
