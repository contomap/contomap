#include "contomap/editor/Editor.h"

using contomap::editor::Editor;
using contomap::model::Contomap;
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
   topic.newName(std::move(name));
   topic.newOccurrence(viewScope, location);
   selection.setForTopic(topic.getId());
}

void Editor::newAssociationRequested(contomap::model::SpacialCoordinate location)
{
   auto &association = map.newAssociation(viewScope, location);
   selection.setForAssociation(association.getId());
}

contomap::model::Identifiers const &Editor::ofViewScope() const
{
   return viewScope;
}

contomap::model::ContomapView const &Editor::ofMap() const
{
   return map;
}
