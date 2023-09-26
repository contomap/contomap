#pragma once

namespace contomap::editor
{

/**
 * SelectionAction defines in which way a selection operation modifies the set of selected items.
 */
enum class SelectionAction
{
   /**
    * Set an item (or items) to be the new selection.
    */
   Set,
   /**
    * Toggle the selected state of an item (or items) within the selection.
    */
   Toggle,
};

} // namespace contomap::frontend
