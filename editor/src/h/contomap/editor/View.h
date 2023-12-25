#pragma once

#include "contomap/editor/Selection.h"
#include "contomap/editor/ViewScope.h"
#include "contomap/model/ContomapView.h"

namespace contomap::editor
{

/**
 * View is the read-only access to the current editor state.
 */
class View
{
public:
   virtual ~View() = default;

   /**
    * @return a view on the current view scope.
    */
   [[nodiscard]] virtual contomap::editor::ViewScope const &ofViewScope() const = 0;

   /**
    * @return a view on the current contomap instance.
    */
   [[nodiscard]] virtual contomap::model::ContomapView const &ofMap() const = 0;

   /**
    * @return a view on the current selection.
    */
   [[nodiscard]] virtual contomap::editor::Selection const &ofSelection() const = 0;
};

} // namespace contomap::editor
