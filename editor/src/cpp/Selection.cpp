#include "contomap/editor/Selection.h"

using contomap::editor::Selection;

void Selection::clear()
{
   topics.clear();
   associations.clear();
}

void Selection::setForTopic(contomap::model::Identifier id)
{
   clear();
   topics.add(id);
}

void Selection::setForAssociation(contomap::model::Identifier id)
{
   clear();
   associations.add(id);
}
