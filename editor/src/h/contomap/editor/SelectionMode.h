#pragma once

namespace contomap::editor
{

/**
 * SelectionMode defines how far a selection operation modifies the set of selected items.
 */
enum class SelectionMode
{
   /**
    * A sole operation considers only the identified item for the operation.
    */
   Sole,
   /**
    * A spread operation considers the identified item, as well as the immediately related items.
    */
   Spread,
};

} // namespace contomap::frontend
