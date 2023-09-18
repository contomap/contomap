#pragma once

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
   [[nodiscard]] virtual contomap::model::Identifiers const &ofViewScope() const = 0;

   /**
    * @return a view on the current contomap instance.
    */
   [[nodiscard]] virtual contomap::model::ContomapView const &ofMap() const = 0;
};

} // namespace contomap::editor
