#include "contomap/editor/Editor.h"

using contomap::editor::Editor;
using contomap::editor::SelectedType;
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
   selection.setSole(SelectedType::Occurrence, occurrence.getId());
}

void Editor::newAssociationRequested(contomap::model::SpacialCoordinate location)
{
   auto &association = map.newAssociation(viewScope, location);
   selection.setSole(SelectedType::Association, association.getId());
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
