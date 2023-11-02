#include "contomap/editor/Editor.h"
#include "contomap/editor/Selections.h"
#include "contomap/model/Topics.h"

using contomap::editor::Editor;
using contomap::editor::SelectedType;
using contomap::editor::SelectionAction;
using contomap::model::Association;
using contomap::model::Contomap;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::SpacialCoordinate;
using contomap::model::Topic;
using contomap::model::TopicNameValue;
using contomap::model::Topics;

Editor::Editor()
   : map(Contomap::newMap())
{
   viewScope.add(map.getDefaultScope());
}

Identifier Editor::newTopicRequested(TopicNameValue name, SpacialCoordinate location)
{
   auto &topic = map.newTopic();
   static_cast<void>(topic.newName(std::move(name)));
   createAndSelectOccurrence(topic, location);
   return topic.getId();
}

void Editor::newOccurrenceRequested(Identifier topicId, SpacialCoordinate location)
{
   auto topic = map.findTopic(topicId);
   if (!topic.has_value())
   {
      return;
   }
   createAndSelectOccurrence(topic.value(), location);
}

Identifier Editor::newAssociationRequested(SpacialCoordinate location)
{
   auto &association = map.newAssociation(viewScope, location);
   selection.setSole(SelectedType::Association, association.getId());
   return association.getId();
}

void Editor::clearSelection()
{
   selection.clear();
}

void Editor::modifySelection(SelectedType type, Identifier id, SelectionAction action)
{
   if (action == SelectionAction::Set)
   {
      selection.setSole(type, id);
   }
   else if (action == SelectionAction::Toggle)
   {
      selection.toggle(type, id);
   }
}

void Editor::linkSelection()
{
   auto &associationIds = selection.of(SelectedType::Association);
   auto &occurrenceIds = selection.of(SelectedType::Occurrence);
   if (!associationIds.empty())
   {
      for (auto associationId : associationIds)
      {
         Association &association = map.findAssociation(associationId).value();
         auto topics = map.find(Topics::thatOccurAs(occurrenceIds));
         for (Topic &topic : topics)
         {
            static_cast<void>(topic.newRole(association));
         }
      }
   }
   else if (!occurrenceIds.empty())
   {
      auto &association = map.newAssociation(viewScope, SpacialCoordinate::absoluteAt(0.0f, 0.0f));
      auto topics = map.find(Topics::thatOccurAs(occurrenceIds));
      float x = 0.0f;
      float y = 0.0f;
      size_t count = 0;
      for (Topic &topic : topics)
      {
         static_cast<void>(topic.newRole(association));

         for (Occurrence const &occurrence : topic.occurrencesIn(viewScope))
         {
            auto occurrenceLocation = occurrence.getLocation().getSpacial().getAbsoluteReference();
            x += occurrenceLocation.X();
            y += occurrenceLocation.Y();
            count++;
         }
      }
      association.moveTo(SpacialCoordinate::absoluteAt(x / static_cast<float>(count), y / static_cast<float>(count)));
      selection.setSole(SelectedType::Association, association.getId());
   }
}

void Editor::deleteSelection()
{
   map.deleteRoles(selection.of(SelectedType::Role));
   map.deleteAssociations(selection.of(SelectedType::Association));
   map.deleteOccurrences(selection.of(SelectedType::Occurrence));
   selection.clear();
   verifyViewScopeIsStable();
}

void Editor::setViewScopeFromSelection()
{
   auto &occurrenceIds = selection.of(SelectedType::Occurrence);
   Identifiers newViewScope;
   for (Topic &topic : map.find(Topics::thatOccurAs(occurrenceIds)))
   {
      newViewScope.add(topic.getId());
   }
   if (!newViewScope.empty())
   {
      setViewScopeTo(newViewScope);
   }
}

void Editor::addToViewScopeFromSelection()
{
   auto &occurrenceIds = selection.of(SelectedType::Occurrence);
   Identifiers newViewScope;
   for (Topic &topic : map.find(Topics::thatOccurAs(occurrenceIds)))
   {
      addToViewScope(topic.getId());
   }
}

void Editor::setViewScopeToDefault()
{
   setViewScopeTo(Identifiers::ofSingle(map.getDefaultScope()));
}

void Editor::setViewScopeTo(Identifier id)
{
   if (!map.findTopic(id).has_value())
   {
      return;
   }
   setViewScopeTo(Identifiers::ofSingle(id));
}

void Editor::addToViewScope(Identifier id)
{
   if (!map.findTopic(id).has_value())
   {
      return;
   }
   viewScope.add(id);
}

void Editor::removeFromViewScope(Identifier id)
{
   if (!viewScope.remove(id))
   {
      return;
   }
   if (viewScope.empty())
   {
      viewScope.add(map.getDefaultScope());
   }
   selection.clear();
}

void Editor::cycleSelectedOccurrenceForward()
{
   cycleSelectedOccurrence(true);
}

void Editor::cycleSelectedOccurrenceReverse()
{
   cycleSelectedOccurrence(false);
}

void Editor::cycleSelectedOccurrence(bool forward)
{
   if (!selection.hasSoleEntryFor(SelectedType::Occurrence))
   {
      return;
   }
   Identifier originalOccurrenceId = *selection.of(SelectedType::Occurrence).begin();
   for (Topic &topic : map.find(Topics::thatOccurAs(Identifiers::ofSingle(originalOccurrenceId))))
   {
      auto const &nextOccurrence = forward ? topic.nextOccurrenceAfter(originalOccurrenceId) : topic.previousOccurrenceBefore(originalOccurrenceId);
      viewScope = nextOccurrence.getScope();
      selection.setSole(SelectedType::Occurrence, nextOccurrence.getId());
   }
}

contomap::model::Identifiers const &Editor::ofViewScope() const
{
   return viewScope;
}

contomap::model::ContomapView const &Editor::ofMap() const
{
   return map;
}

contomap::editor::Selection const &Editor::ofSelection() const
{
   return selection;
}

void Editor::createAndSelectOccurrence(contomap::model::Topic &topic, contomap::model::SpacialCoordinate location)
{
   auto &occurrence = topic.newOccurrence(viewScope, location);
   selection.setSole(SelectedType::Occurrence, occurrence.getId());
}

void Editor::setViewScopeTo(contomap::model::Identifiers const &ids)
{
   viewScope = ids;
   selection.clear();
}

void Editor::verifyViewScopeIsStable()
{
   Identifiers unknownIds;

   for (auto id : viewScope)
   {
      if (!map.findTopic(id).has_value())
      {
         unknownIds.add(id);
      }
   }
   for (auto id : unknownIds)
   {
      removeFromViewScope(id);
   }
}
