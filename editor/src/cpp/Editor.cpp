#include "contomap/editor/Editor.h"

using contomap::editor::Editor;
using contomap::model::Contomap;
using contomap::model::SpacialCoordinate;
using contomap::model::TopicNameValue;

void Editor::newTopicRequested(TopicNameValue name, SpacialCoordinate location)
{
   auto &topic = map.newTopic();
   topic.newName(std::move(name));
   topic.newOccurrence(viewScope, location);
}
