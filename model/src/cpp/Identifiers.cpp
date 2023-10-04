#include <algorithm>

#include "contomap/model/Identifiers.h"

using contomap::model::Identifier;
using contomap::model::Identifiers;

Identifiers Identifiers::ofSingle(Identifier id)
{
   Identifiers ids;
   ids.add(id);
   return ids;
}

Identifiers::CollectionType::const_iterator Identifiers::begin() const
{
   return set.begin();
}

Identifiers::CollectionType::const_iterator Identifiers::end() const
{
   return set.end();
}

void Identifiers::add(Identifier id)
{
   set.insert(id);
}

bool Identifiers::remove(Identifier id)
{
   return set.erase(id) > 0;
}

void Identifiers::clear()
{
   set.clear();
}

bool Identifiers::empty() const
{
   return set.empty();
}

bool Identifiers::contains(Identifier id) const
{
   return set.contains(id);
}

bool Identifiers::contains(Identifiers const &other) const
{
   return std::all_of(other.begin(), other.end(), [this](auto const &id) { return set.contains(id); });
}
