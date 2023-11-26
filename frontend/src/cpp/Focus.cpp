#include <limits>

#include "contomap/frontend/Focus.h"

using contomap::editor::InputRequestHandler;
using contomap::editor::SelectionAction;
using contomap::frontend::Focus;
using contomap::frontend::FocusItem;

Focus::Focus()
   : distance(std::numeric_limits<float>::max())
{
}

void Focus::registerItem(std::shared_ptr<FocusItem> newItem, float newDistance)
{
   if (newDistance < distance)
   {
      item = std::move(newItem);
      distance = newDistance;
   }
}

void Focus::modifySelection(InputRequestHandler &handler, SelectionAction action) const
{
   if (item != nullptr)
   {
      item->modifySelection(handler, action);
   }
   else
   {
      handler.clearSelection();
   }
}
