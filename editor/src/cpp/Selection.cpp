#include <algorithm>
#include <numeric>

#include "contomap/editor/Selection.h"

using contomap::editor::SelectedType;
using contomap::editor::Selection;
using contomap::infrastructure::LinkedReferences;
using contomap::infrastructure::Sized;
using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Association;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::Role;

template <> SelectedType const Selection::Marker<Occurrence>::TYPE = SelectedType::Occurrence;
template <> SelectedType const Selection::Marker<Association>::TYPE = SelectedType::Association;
template <> SelectedType const Selection::Marker<Role>::TYPE = SelectedType::Role;

Selection::Selection()
   : lists {
      LinkedReferences<Occurrence>(),
      LinkedReferences<Association>(),
      LinkedReferences<Role>(),
   }
{
}

Selection Selection::from(Decoder &coder, uint8_t, std::function<Occurrence &(Identifier)> const &occurrenceResolver,
   std::function<Association &(Identifier)> const &associationResolver, std::function<Role &(Identifier)> const &roleResolver)
{
   Selection instance;
   Coder::Scope scope(coder, "selection");
   instance.decodeList<Occurrence>(coder, "occurrences", occurrenceResolver);
   instance.decodeList<Association>(coder, "associations", associationResolver);
   instance.decodeList<Role>(coder, "roles", roleResolver);
   /*
   auto decode = [&instance, &coder]<class T>(SelectedType type, std::string const &name, std::function<T &(Identifier)> resolver) {
      instance.identifiers[type].decode(coder, name);
      for (Identifier id : instance.identifiers[type])
      {
         resolver(id);
      }
   };
   decode(SelectedType::Occurrence, "occurrences", occurrenceResolver);
   decode(SelectedType::Association, "associations", associationResolver);
   decode(SelectedType::Role, "roles", roleResolver);
   */
   return instance;
}

void Selection::encode(Encoder &coder) const
{
   Coder::Scope scope(coder, "selection");
   /*
   auto encodeList = [this, &coder]<class T>(std::string const &name, T *ptr = nullptr) {
      auto &list = listFor<T>();

      for (auto const &ref : list.allReferences())
      {
      }
   };
   */
   encodeList<Occurrence>(coder, "occurrences");
   encodeList<Association>(coder, "associations");
   encodeList<Role>(coder, "roles");
   /*
   static Identifiers const EMPTY;
   auto encode = [this, &coder](SelectedType type, std::string const &name) {
      auto const &ids = identifiers.contains(type) ? identifiers.at(type) : EMPTY;
      ids.encode(coder, name);
   };
   encode(SelectedType::Occurrence, "occurrences");
   encode(SelectedType::Association, "associations");
   encode(SelectedType::Role, "roles");
   */
}

bool Selection::empty() const
{
   return std::all_of(lists.begin(), lists.end(), [](auto const &list) { return asSized(list).size() == 0; });
}

void Selection::clear()
{
   for (auto &list : lists)
   {
      asSized(list).clear();
   }
   // lists.clear();
}

bool Selection::hasSoleEntry() const
{
   return std::accumulate(lists.begin(), lists.end(), 0, [](auto acc, auto const &list) { return acc + asSized(list).size(); }) == 1;
}

bool Selection::hasSoleEntryFor(SelectedType type) const
{
   for (size_t i = 0; i < lists.size(); i++)
   {
      auto otherType = static_cast<SelectedType>(i);
      auto const &sized = asSized(lists[i]);
      if (((otherType == type) && (sized.size() != 1)) || ((otherType != type) && (sized.size() != 0)))
      {
         return false;
      }
   }
   return true;
   /*
   return std::all_of(lists.begin(), lists.end(), [type](auto const &list) {
      auto otherType = kvp.first;
      auto const &sized = asSized(*kvp.second);
      return ((otherType == type) && (sized.size() == 1)) || ((otherType != type) && (sized.size() == 0));
   });
   */
}
/*
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
*/

/*
Identifiers const &Selection::of(SelectedType type) const
{
   static Identifiers const empty;
   auto it = identifiers.find(type);
   return (it != identifiers.end()) ? it->second : empty;
}
*/

Sized const &Selection::asSized(SelectionList const &list)
{
   std::optional<std::reference_wrapper<Sized const>> optionalSized;
   std::visit([&optionalSized](Sized const &sized) { optionalSized = sized; }, list);
   return optionalSized.value();
}

Sized &Selection::asSized(SelectionList &list)
{
   std::optional<std::reference_wrapper<Sized>> optionalSized;
   std::visit([&optionalSized](Sized &sized) { optionalSized = sized; }, list);
   return optionalSized.value();
}
