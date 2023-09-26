#include "contomap/editor/Editor.h"

using contomap::editor::Editor;
using contomap::editor::SelectionAction;
using contomap::editor::SelectionMode;
using contomap::model::Contomap;
using contomap::model::Identifier;
using contomap::model::SpacialCoordinate;
using contomap::model::TopicNameValue;

Editor::Editor()
   : map(Contomap::newMap())
{
   viewScope.add(map.getDefaultScope());
}

void Editor::newTopicRequested(TopicNameValue name, SpacialCoordinate location)
{
   auto &topic = map.newTopic();
   static_cast<void>(topic.newName(std::move(name)));
   auto &occurrence = topic.newOccurrence(viewScope, location);
   selection.setForOccurrence(occurrence.getId());
}

void Editor::newAssociationRequested(contomap::model::SpacialCoordinate location)
{
   auto &association = map.newAssociation(viewScope, location);
   selection.setForAssociation(association.getId());
}

void Editor::modifySelectionOfOccurrence(Identifier id, SelectionAction action, SelectionMode mode)
{
   // TODO if/elseif might not be best approach. See if some strategy approach can help.

   if (mode == SelectionMode::Sole)
   {
      if (action == SelectionAction::Set)
      {
         selection.setForOccurrence(id);
      }
      else if (action == SelectionAction::Toggle)
      {
         selection.toggleOccurrence(id);
      }
   }
}

void Editor::modifySelectionOfAssociation(Identifier id, SelectionAction action, SelectionMode mode)
{
   // TODO this is a lot of duplication for a second type. When roles/players will be added, this is even more duplication.

   if (mode == SelectionMode::Sole)
   {
      if (action == SelectionAction::Set)
      {
         selection.setForAssociation(id);
      }
      else if (action == SelectionAction::Toggle)
      {
         selection.toggleAssociation(id);
      }
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
