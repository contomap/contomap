#include "contomap/editor/Selection.h"

using contomap::editor::Selection;
using contomap::model::Identifier;

void Selection::clear()
{
   occurrences.clear();
   associations.clear();
}

void Selection::setForOccurrence(Identifier id)
{
   clear();
   occurrences.add(id);
}

void Selection::toggleOccurrence(contomap::model::Identifier id)
{
   if (occurrences.contains(id))
   {
      occurrences.remove(id);
   }
   else
   {
      occurrences.add(id);
   }
}

bool Selection::containsOccurrence(Identifier id) const
{
   return occurrences.contains(id);
}

void Selection::setForAssociation(Identifier id)
{
   clear();
   associations.add(id);
}

void Selection::toggleAssociation(contomap::model::Identifier id)
{
   if (associations.contains(id))
   {
      associations.remove(id);
   }
   else
   {
      associations.add(id);
   }
}

bool Selection::containsAssociation(Identifier id) const
{
   return associations.contains(id);
}
