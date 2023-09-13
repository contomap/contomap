#pragma once

#include <raygui/raygui.h>

namespace contomap::frontend
{

/**
 * RenderContext provides information about the current display properties.
 */
class RenderContext
{
public:
   /**
    * @return the context for the current UI state.
    */
   static RenderContext fromCurrentState();

   /**
    * @return the size of the content within the view.
    */
   [[nodiscard]] Vector2 getContentSize() const;

private:
   RenderContext(Vector2 dpiScale, Vector2 renderSize);

   Vector2 dpiScale;
   Vector2 renderSize;
};

} // namespace contomap::frontend
