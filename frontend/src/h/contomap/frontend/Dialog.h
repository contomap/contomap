#pragma once

#include "contomap/frontend/RenderContext.h"

namespace contomap::frontend
{

/**
 * Dialog is an interface for modal dialogs.
 */
class Dialog
{
public:
   virtual ~Dialog() = default;

   /**
    * Requests to draw the dialog.
    *
    * @param context the context to draw within.
    * @return true if the dialog shall be closed.
    */
   [[nodiscard]] virtual bool draw(contomap::frontend::RenderContext const &context) = 0;
};

} // namespace contomap::frontend
