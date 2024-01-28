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

std::optional<std::reference_wrapper<Occurrence const>> Selections::firstOccurrenceFrom(Selection const &selection, ContomapView const &view)
{
   for (Occurrence const &occurrence : selection.of<Occurrence>())
   {
      return occurrence;
   }
   return {};
}

std::optional<std::reference_wrapper<Topic const>> Selections::topicOfFirstOccurrenceFrom(Selection const &selection, ContomapView const &view)
{
   for (Occurrence const &occurrence : selection.of<Occurrence>())
   {
      return occurrence.getTopic();
   }
   return {};
}

std::optional<Style> Selections::firstAppearanceFrom(Selection const &selection, ContomapView const &view)
{
   for (Styleable const &styleable : selection.of<Occurrence>())
   {
      return styleable.getAppearance();
   }
   for (Styleable const &styleable : selection.of<Association>())
   {
      return styleable.getAppearance();
   }
   for (Styleable const &styleable : selection.of<Role>())
   {
      return styleable.getAppearance();
   }
   return {};
}

std::optional<std::reference_wrapper<Reifiable<Topic> const>> Selections::firstReifiableFrom(Selection const &selection, ContomapView const &view)
{
   for (Reifiable<Topic> const &reifiable : selection.of<Occurrence>())
   {
      return reifiable;
   }
   for (Reifiable<Topic> const &reifiable : selection.of<Association>())
   {
      return reifiable;
   }
   for (Reifiable<Topic> const &reifiable : selection.of<Role>())
   {
      return reifiable;
   }
   return {};
}

Search<contomap::model::Typeable> Selections::allTypeableFrom(contomap::editor::Selection const &selection, contomap::model::Contomap &map)
{
   for (auto &occurrence : selection.of<Occurrence>())
   {
      co_yield occurrence;
   }
   for (auto &association : selection.of<Association>())
   {
      co_yield association;
   }
   for (auto &role : selection.of<Role>())
   {
      co_yield role;
   }
}

Search<contomap::model::Reifiable<Topic>> Selections::allReifiableFrom(contomap::editor::Selection const &selection, contomap::model::Contomap &map)
{
   for (auto &occurrence : selection.of<Occurrence>())
   {
      co_yield occurrence;
   }
   for (auto &association : selection.of<Association>())
   {
      co_yield association;
   }
   for (auto &role : selection.of<Role>())
   {
      co_yield role;
   }
}

Search<contomap::model::Styleable> Selections::allStyleableFrom(contomap::editor::Selection const &selection, contomap::model::Contomap &map)
{
   for (auto &occurrence : selection.of<Occurrence>())
   {
      co_yield occurrence;
   }
   for (auto &association : selection.of<Association>())
   {
      co_yield association;
   }
   for (auto &role : selection.of<Role>())
   {
      co_yield role;
   }
}
