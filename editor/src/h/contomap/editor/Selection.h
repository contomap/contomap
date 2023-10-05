#pragma once

#include <map>

#include "contomap/editor/SelectedType.h"
#include "contomap/model/Identifiers.h"

namespace contomap::editor
{

/**
 * Selection keeps track of items that are currently highlighted.
 */
class Selection
{
public:
   /**
    * Default constructor.
    */
   Selection() = default;

   /**
    * Empties the selection.
    */
   void clear();

   /**
    * Set the sole selection to be for the identified type.
    *
    * @param type the type that the identifier refers to.
    * @param id identifier of the selected thing.
    */
   void setSole(contomap::editor::SelectedType type, contomap::model::Identifier id);

   /**
    * Toggles the selection of the identified thing.
    *
    * @param type the type that the identifier refers to.
    * @param id identifier for the selected thing.
    */
   void toggle(contomap::editor::SelectedType type, contomap::model::Identifier id);

   /**
    * Determines whether a specific thing is part of the selection.
    *
    * @param type the type that the identifier refers to.
    * @param id identifier for the occurrence to check.
    * @return true if the identified thing is in the selection.
    */
   [[nodiscard]] bool contains(contomap::editor::SelectedType type, contomap::model::Identifier id) const;

   /**
    * Provide the set of selected identifiers for given type.
    *
    * @param type the type to filter for.
    * @return a set of identifiers for the given selected type.
    */
   [[nodiscard]] contomap::model::Identifiers const &of(contomap::editor::SelectedType type) const;

private:
   std::map<contomap::editor::SelectedType, contomap::model::Identifiers> identifiers;
};

} // namespace contomap::frontend
