#pragma once

#include "contomap/editor/Selection.h"
#include "contomap/model/ContomapView.h"

namespace contomap::editor
{

/**
 * Selections provides helper functions for resolving items.
 */
class Selections
{
public:
   /**
    * Retrieves the first occurrence from the selection.
    *
    * @param selection the selection containing selection identifier(s).
    * @param view the view through which to resolve the objects.
    * @return the reference to the found occurrence - if selected and existing.
    */
   [[nodiscard]] static std::optional<std::reference_wrapper<contomap::model::Occurrence const>> firstOccurrenceFrom(
      contomap::editor::Selection const &selection, contomap::model::ContomapView const &view);

   /**
    * Retrieves the first topic from the selection of occurrences.
    *
    * @param selection the selection containing selection identifier(s).
    * @param view the view through which to resolve the objects.
    * @return the reference to the found topic - if selected and existing.
    */
   [[nodiscard]] static std::optional<std::reference_wrapper<contomap::model::Topic const>> topicOfFirstOccurrenceFrom(
      contomap::editor::Selection const &selection, contomap::model::ContomapView const &view);
};

} // namespace contomap::editor
