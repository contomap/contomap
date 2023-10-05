#include "contomap/editor/Editor.h"
#include "contomap/model/Topics.h"

using contomap::editor::Editor;
using contomap::editor::SelectedType;
using contomap::editor::SelectionAction;
using contomap::model::Association;
using contomap::model::Contomap;
using contomap::model::Identifier;
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
   auto &occurrence = topic.newOccurrence(viewScope, location);
   selection.setSole(SelectedType::Occurrence, occurrence.getId());
   return topic.getId();
}

void Editor::newOccurrenceRequested(Identifier topicId, SpacialCoordinate location)
{
   auto topic = map.findTopic(topicId);
   if (!topic.has_value())
   {
      return;
   }
   auto &occurrence = topic.value().get().newOccurrence(viewScope, location);
   selection.setSole(SelectedType::Occurrence, occurrence.getId());
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
         }
         count++;
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
