#pragma once

#include <array>

#include "contomap/frontend/Dialog.h"
#include "contomap/frontend/DisplayEnvironment.h"
#include "contomap/frontend/Layout.h"

namespace contomap::frontend
{

/**
 * SaveAsDialog queries for a file name.
 */
class SaveAsDialog : public contomap::frontend::Dialog
{
public:
   /**
    * Constructor.
    *
    * @param environment the environment to use for requesting native dialogs.
    * @param layout the layout to work with when drawing.
    * @param proposedFilePath the file path to use as a proposal.
    */
   SaveAsDialog(contomap::frontend::DisplayEnvironment &environment, contomap::frontend::Layout const &layout, std::string const &proposedFilePath);
   ~SaveAsDialog() override = default;

   bool draw(contomap::frontend::RenderContext const &context) override;

private:
   contomap::frontend::DisplayEnvironment &environment;
   contomap::frontend::Layout layout;
   std::string proposedFilePath;
   std::array<char, 1024> newFilePath {};
};

} // namespace contomap::frontend
