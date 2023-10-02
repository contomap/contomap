#include "contomap/editor/Editor.h"
#include "contomap/model/Topics.h"

using contomap::editor::Editor;
using contomap::editor::SelectedType;
using contomap::editor::SelectionAction;
using contomap::editor::SelectionMode;
using contomap::model::Association;
using contomap::model::Contomap;
using contomap::model::Identifier;
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

Identifier Editor::newAssociationRequested(contomap::model::SpacialCoordinate location)
{
   auto &association = map.newAssociation(viewScope, location);
   selection.setSole(SelectedType::Association, association.getId());
   return association.getId();
}

void Editor::clearSelection()
{
   selection.clear();
}

void Editor::modifySelection(SelectedType type, Identifier id, SelectionAction action, SelectionMode mode)
{
   // TODO if/elseif might not be best approach. See if some strategy approach can help.

   // mode is a strategy that provides a list of implicitly included things, based on ContomapView
   // action then operates on Selection object

   if (mode == SelectionMode::Sole)
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
   else if (mode == SelectionMode::Spread)
   {
      if (action == SelectionAction::Set)
      {
         // on occurrence: Set occurrence, and all its (currently scoped) associations and the related roles
         // on association: Set association, and all its (currently scoped) occurrences and the related roles
         // on role: Set role, and its (currently scoped) association and occurrences
      }
      else if (action == SelectionAction::Toggle)
      {
         // for the selected type, do the inverse to its current state to itself, and align all the related things, as in Set branch.
      }
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
      // TODO: create association in the middle of all occurrences, create roles.
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
