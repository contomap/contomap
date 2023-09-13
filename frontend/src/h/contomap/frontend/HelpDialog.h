#pragma once

#include "contomap/frontend/Dialog.h"

namespace contomap::frontend
{

/**
 * HelpDialog shows some basic information of the application.
 */
class HelpDialog : public contomap::frontend::Dialog
{
public:
   ~HelpDialog() override = default;

   bool draw(contomap::frontend::RenderContext const &context) override;
};

} // namespace contomap::frontend
