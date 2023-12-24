#include <algorithm>

#include "contomap/editor/Selection.h"

using contomap::editor::SelectedType;
using contomap::editor::Selection;
using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Association;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::Role;

Selection Selection::from(Decoder &coder, uint8_t, std::function<Occurrence &(Identifier)> const &, std::function<Association &(Identifier)> const &,
   std::function<Role &(Identifier)> const &)
{
   Selection instance;
   Coder::Scope scope(coder, "selection");
   auto decode = [&instance, &coder](SelectedType type, std::string const &name) { instance.identifiers[type].decode(coder, name); };
   decode(SelectedType::Occurrence, "occurrences");
   decode(SelectedType::Association, "associations");
   decode(SelectedType::Role, "roles");
   return instance;
}

void Selection::encode(Encoder &coder) const
{
   static Identifiers const EMPTY;
   Coder::Scope scope(coder, "selection");
   auto encode = [this, &coder](SelectedType type, std::string const &name) {
      auto const &ids = identifiers.contains(type) ? identifiers.at(type) : EMPTY;
      ids.encode(coder, name);
   };
   encode(SelectedType::Occurrence, "occurrences");
   encode(SelectedType::Association, "associations");
   encode(SelectedType::Role, "roles");
}

bool Selection::empty() const
{
   return std::all_of(identifiers.begin(), identifiers.end(), [](auto const &kvp) { return kvp.second.empty(); });
}

void Selection::clear()
{
   identifiers.clear();
}

bool Selection::hasSoleEntry() const
{
   bool foundOne = false;
   for (auto const &kvp : identifiers)
   {
      auto const &specific = kvp.second;
      size_t size = specific.size();
      if (size > 1)
      {
         return false;
      }
      else if (size == 1)
      {
         if (foundOne)
         {
            return false;
         }
         foundOne = true;
      }
   }
   return foundOne;
}

bool Selection::hasSoleEntryFor(SelectedType type) const
{
   return identifiers.contains(type) && std::all_of(identifiers.begin(), identifiers.end(), [type](auto const &kvp) {
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
