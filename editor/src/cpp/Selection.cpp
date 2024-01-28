#include <algorithm>
#include <numeric>

#include "contomap/editor/Selection.h"

using contomap::editor::SelectedType;
using contomap::editor::Selection;
using contomap::infrastructure::LinkedReferences;
using contomap::infrastructure::References;
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
   return instance;
}

void Selection::encode(Encoder &coder) const
{
   Coder::Scope scope(coder, "selection");
   encodeList<Occurrence>(coder, "occurrences");
   encodeList<Association>(coder, "associations");
   encodeList<Role>(coder, "roles");
}

bool Selection::empty() const
{
   return std::all_of(lists.begin(), lists.end(), [](auto const &list) { return asReferences(list).size() == 0; });
}

void Selection::clear()
{
   for (auto &list : lists)
   {
      asReferences(list).clear();
   }
}

bool Selection::hasSoleEntry() const
{
   return std::accumulate(lists.begin(), lists.end(), 0, [](auto acc, auto const &list) { return acc + asReferences(list).size(); }) == 1;
}

bool Selection::hasSoleEntryFor(SelectedType type) const
{
   for (size_t i = 0; i < lists.size(); i++)
   {
      auto otherType = static_cast<SelectedType>(i);
      auto const &sized = asReferences(lists[i]);
      if (((otherType == type) && (sized.size() != 1)) || ((otherType != type) && (sized.size() != 0)))
      {
         return false;
      }
   }
   return true;
}

References const &Selection::asReferences(SelectionList const &list)
{
   std::optional<std::reference_wrapper<References const>> optionalSized;
   std::visit([&optionalSized](References const &sized) { optionalSized = sized; }, list);
   return optionalSized.value();
}

References &Selection::asReferences(SelectionList &list)
{
   std::optional<std::reference_wrapper<References>> optionalSized;
   std::visit([&optionalSized](References &sized) { optionalSized = sized; }, list);
   return optionalSized.value();
}
