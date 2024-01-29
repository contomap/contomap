#include "contomap/editor/Selections.h"
#include "contomap/model/Topics.h"

using contomap::editor::SelectedType;
using contomap::editor::Selection;
using contomap::editor::Selections;
using contomap::infrastructure::Search;
using contomap::model::Association;
using contomap::model::Contomap;
using contomap::model::ContomapView;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::Reifiable;
using contomap::model::Role;
using contomap::model::Style;
using contomap::model::Styleable;
using contomap::model::Topic;
using contomap::model::Topics;
using contomap::model::Typeable;

std::optional<std::reference_wrapper<Occurrence const>> Selections::firstOccurrenceFrom(Selection const &selection)
{
   for (Occurrence const &occurrence : selection.of<Occurrence>())
   {
      return occurrence;
   }
   return {};
}

std::optional<std::reference_wrapper<Topic const>> Selections::topicOfFirstOccurrenceFrom(Selection const &selection)
{
   for (Occurrence const &occurrence : selection.of<Occurrence>())
   {
      return occurrence.getTopic();
   }
   return {};
}

std::optional<Style> Selections::firstAppearanceFrom(Selection const &selection)
{
   for (Styleable const &styleable : allStyleableFrom(selection))
   {
      return styleable.getAppearance();
   }
   return {};
}

std::optional<std::reference_wrapper<Reifiable<Topic> const>> Selections::firstReifiableFrom(Selection const &selection)
{
   for (Reifiable<Topic> const &reifiable : allReifiableFrom(selection))
   {
      return reifiable;
   }
   return {};
}

Search<Typeable> Selections::allTypeableFrom(contomap::editor::Selection const &selection)
{
   return allFrom<Typeable, Typeable, Occurrence, Association, Role>(selection, [](Typeable &t) -> Typeable & { return t; });
}

Search<contomap::model::Reifiable<Topic>> Selections::allReifiableFrom(contomap::editor::Selection const &selection)
{
   return allFrom<Reifiable<Topic>, Reifiable<Topic>, Occurrence, Association, Role>(selection, [](Reifiable<Topic> &r) -> Reifiable<Topic> & { return r; });
}

Search<Styleable> Selections::allStyleableFrom(contomap::editor::Selection const &selection)
{
   return allFrom<Styleable, Styleable, Occurrence, Association, Role>(selection, [](Styleable &s) -> Styleable & { return s; });
}
