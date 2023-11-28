#include <algorithm>
#include <vector>

#include "contomap/model/Identifiers.h"

using contomap::infrastructure::serial::Coder;
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

void Identifiers::code(std::string const &name, Coder &coder)
{
   // As the content of a set can not be modified, a temporary object needs to cover for it.
   std::vector<Identifier> buf(set.begin(), set.end());
   coder.code(name, buf.size(), [&buf](size_t index, Coder &nested) {
      if (index < buf.size())
      {
         buf[index].code("", nested);
      }
      else
      {
         buf.emplace_back(Identifier::from("", nested));
      }
   });
   set = CollectionType(buf.begin(), buf.end());
}
