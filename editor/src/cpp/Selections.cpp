#include "contomap/editor/Selections.h"
#include "contomap/model/Topics.h"

using contomap::editor::SelectedType;
using contomap::editor::Selection;
using contomap::editor::Selections;
using contomap::model::ContomapView;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
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
   if (auto const &occurrenceIds = selection.of(SelectedType::Occurrence); !occurrenceIds.empty())
   {
      for (auto const &occurrence : view.findOccurrences(occurrenceIds))
      {
         return occurrence.get().getAppearance();
      }
   }
   // TODO: add further types
   return {};
}
