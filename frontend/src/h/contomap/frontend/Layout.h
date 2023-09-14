#pragma once

#include "contomap/frontend/RenderContext.h"

namespace contomap::frontend
{

/**
 * Layout provides constants and calculating functions.
 */
class Layout
{
public:
   /**
    * @return the height of a window title bar to skip.
    */
   [[nodiscard]] float windowTitleHeight() const
   {
      return 24.0f;
   }

   /**
    * @return the distance between elements.
    */
   [[nodiscard]] float padding() const
   {
      return 2.0f;
   }

   /**
    * @return the height of a typical text line.
    */
   [[nodiscard]] float lineHeight() const
   {
      return 20.0f;
   }

   /**
    * @return the height of a typical button.
    */
   [[nodiscard]] float buttonHeight() const
   {
      return 24.0f;
   }
};

} // namespace contomap::frontend
