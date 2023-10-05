#pragma once

#include "contomap/frontend/Dialog.h"
#include "contomap/frontend/Layout.h"

namespace contomap::frontend
{

/**
 * HelpDialog shows some basic information of the application.
 */
class HelpDialog : public contomap::frontend::Dialog
{
public:
   /**
    * Constructor.
    *
    * @param layout the layout to work with when drawing.
    */
   explicit HelpDialog(contomap::frontend::Layout const &layout);
   ~HelpDialog() override = default;

   bool draw(contomap::frontend::RenderContext const &context) override;

private:
   contomap::frontend::Layout layout;
};

} // namespace contomap::frontend
