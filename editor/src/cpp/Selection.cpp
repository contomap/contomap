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
using contomap::model::Role;
using contomap::model::Topic;

Selection Selection::from(Decoder &coder, uint8_t, std::function<Topic &(Identifier)> const &, std::function<Association &(Identifier)> const &,
   std::function<Role &(Identifier)> const &)
{
   Selection instance;
   Coder::Scope scope(coder, "selection");
   coder.codeArray("identifiers", [&instance](Decoder &nested, size_t) {
      Coder::Scope entryScope(nested, "");
      uint8_t serialType = 0x00;
      nested.code("type", serialType);
      Identifiers ids;
      ids.decode(nested, "ids");
      instance.identifiers.emplace(typeFromSerial(serialType), ids);
   });
   return instance;
}

void Selection::encode(Encoder &coder) const
{
   Coder::Scope scope(coder, "selection");
   coder.codeArray("identifiers", identifiers.begin(), identifiers.end(), [](Encoder &nested, auto const &kvp) {
      Coder::Scope entryScope(nested, "");
      nested.code("type", typeToSerial(kvp.first));
      kvp.second.encode(nested, "ids");
   });
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
   for (auto const &[_, specific] : identifiers)
   {
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

SelectedType Selection::typeFromSerial(uint8_t value)
{
   static std::map<uint8_t, SelectedType> CONSTANTS { { 0x01, SelectedType::Association }, { 0x02, SelectedType::Occurrence }, { 0x03, SelectedType::Role } };
   return CONSTANTS.at(value);
}

uint8_t Selection::typeToSerial(SelectedType type)
{
   static std::map<SelectedType, uint8_t> CONSTANTS { { SelectedType::Association, 0x01 }, { SelectedType::Occurrence, 0x02 }, { SelectedType::Role, 0x03 } };
   return CONSTANTS.at(type);
}
