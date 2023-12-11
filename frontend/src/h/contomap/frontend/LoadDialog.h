#pragma once

#include <array>
#include <functional>
#include <string>

#include "contomap/frontend/Dialog.h"
#include "contomap/frontend/DisplayEnvironment.h"
#include "contomap/frontend/Layout.h"

namespace contomap::frontend
{

/**
 * LoadDialog queries for a file name.
 */
class LoadDialog : public contomap::frontend::Dialog
{
public:
   /** Function to be called upon completion. */
   using LoadFunction = std::function<void(std::string const &)>;

   /**
    * Constructor.
    *
    * @param environment the environment to use for requesting native dialogs.
    * @param layout the layout to work with when drawing.
    * @param loadFunction the function to call on confirmation.
    */
   LoadDialog(contomap::frontend::DisplayEnvironment &environment, contomap::frontend::Layout const &layout, LoadFunction loadFunction);
   ~LoadDialog() override = default;

   bool draw(contomap::frontend::RenderContext const &context) override;

private:
   contomap::frontend::DisplayEnvironment &environment;
   contomap::frontend::Layout layout;
   LoadFunction loadFunction;
   std::array<char, 1024> newFilePath {};
};

} // namespace contomap::frontend
