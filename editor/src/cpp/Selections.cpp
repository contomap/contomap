#include "contomap/editor/Selections.h"
#include "contomap/model/Topics.h"

using contomap::editor::SelectedType;
using contomap::editor::Selection;
using contomap::editor::Selections;
using contomap::infrastructure::Search;
using contomap::model::Contomap;
using contomap::model::ContomapView;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::Reifiable;
using contomap::model::Style;
using contomap::model::Topic;
using contomap::model::Topics;

std::optional<std::reference_wrapper<Occurrence const>> Selections::firstOccurrenceFrom(Selection const &selection, ContomapView const &view)
{
   auto const &occurrences = selection.of(SelectedType::Occurrence);
   auto first = occurrences.begin();
   if (first == occurrences.end())
   {
      return {};
   }
   Identifier occurrenceId = *first;
   auto topics = view.find(Topics::thatOccurAs(Identifiers::ofSingle(occurrenceId)));
   auto firstTopic = topics.begin();
   if (firstTopic == topics.end())
   {
      return {};
   }
   return (*firstTopic).get().getOccurrence(occurrenceId);
}

std::optional<std::reference_wrapper<Topic const>> Selections::topicOfFirstOccurrenceFrom(Selection const &selection, ContomapView const &view)
{
   auto const &occurrences = selection.of(SelectedType::Occurrence);
   auto first = occurrences.begin();
   if (first == occurrences.end())
   {
      return {};
   }
   Identifier occurrenceId = *first;
   auto topics = view.find(Topics::thatOccurAs(Identifiers::ofSingle(occurrenceId)));
   auto firstTopic = topics.begin();
   if (firstTopic == topics.end())
   {
      return {};
   }
   return { (*firstTopic).get() };
}

std::optional<Style> Selections::firstAppearanceFrom(Selection const &selection, ContomapView const &view)
{
   if (auto const &ids = selection.of(SelectedType::Occurrence); !ids.empty())
   {
      for (auto const &occurrence : view.findOccurrences(ids))
      {
         return occurrence.get().getAppearance();
      }
   }
   if (auto const &ids = selection.of(SelectedType::Association); !ids.empty())
   {
      auto const &association = view.findAssociation(*ids.begin());
      return association.value().get().getAppearance();
   }
   if (auto const &ids = selection.of(SelectedType::Role); !ids.empty())
   {
      for (auto const &role : view.findRoles(ids))
      {
         return role.get().getAppearance();
      }
   }
   return {};
}

std::optional<std::reference_wrapper<Reifiable<Topic> const>> Selections::firstReifiableFrom(Selection const &selection, ContomapView const &view)
{
   if (auto const &ids = selection.of(SelectedType::Occurrence); !ids.empty())
   {
      for (auto const &occurrence : view.findOccurrences(ids))
      {
         return occurrence.get();
      }
   }
   if (auto const &ids = selection.of(SelectedType::Association); !ids.empty())
   {
      auto const &association = view.findAssociation(*ids.begin());
      return association.value().get();
   }
   if (auto const &ids = selection.of(SelectedType::Role); !ids.empty())
   {
      for (auto const &role : view.findRoles(ids))
      {
         return role.get();
      }
   }
   return {};
}

Search<contomap::model::Typeable> Selections::allTypeableFrom(contomap::editor::Selection const &selection, contomap::model::Contomap &map)
{
   for (auto &occurrence : selectedOccurrences(selection, map))
   {
      co_yield occurrence;
   }
   for (auto &association : selectedAssociations(selection, map))
   {
      co_yield association;
   }
   for (auto &role : selectedRoles(selection, map))
   {
      co_yield role;
   }
}

Search<contomap::model::Reifiable<Topic>> Selections::allReifiableFrom(contomap::editor::Selection const &selection, contomap::model::Contomap &map)
{
   for (auto &occurrence : selectedOccurrences(selection, map))
   {
      co_yield occurrence;
   }
   for (auto &association : selectedAssociations(selection, map))
   {
      co_yield association;
   }
   for (auto &role : selectedRoles(selection, map))
   {
      co_yield role;
   }
}

Search<contomap::model::Styleable> Selections::allStyleableFrom(contomap::editor::Selection const &selection, contomap::model::Contomap &map)
{
   for (auto &occurrence : selectedOccurrences(selection, map))
   {
      co_yield occurrence;
   }
   for (auto &association : selectedAssociations(selection, map))
   {
      co_yield association;
   }
   for (auto &role : selectedRoles(selection, map))
   {
      co_yield role;
   }
}

Search<contomap::model::Occurrence> Selections::selectedOccurrences(Selection const &selection, Contomap &map)
{
   return map.findOccurrences(selection.of(SelectedType::Occurrence));
}

Search<contomap::model::Association> Selections::selectedAssociations(Selection const &selection, Contomap &map)
{
   for (auto id : selection.of(SelectedType::Association))
   {
      co_yield map.findAssociation(id).value();
   }
}

Search<contomap::model::Role> Selections::selectedRoles(Selection const &selection, Contomap &map)
{
   return map.findRoles(selection.of(SelectedType::Role));
}
