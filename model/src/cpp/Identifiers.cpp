#include <algorithm>
#include <vector>

#include "contomap/model/Identifiers.h"

using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
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

size_t Identifiers::size() const
{
   return set.size();
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

void Identifiers::encode(Encoder &encoder, std::string const &name) const
{
   encoder.codeArray(name, set.begin(), set.end(), [](Encoder &nested, Identifier const &id) { id.encode(nested, ""); });
}

void Identifiers::decode(Decoder &decoder, std::string const &name)
{
   decoder.codeArray(name, [this](Decoder &nested, size_t) { set.emplace(Identifier::from(nested, "")); });
}
