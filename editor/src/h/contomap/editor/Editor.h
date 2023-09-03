#pragma once

#include "contomap/editor/InputRequestHandler.h"

namespace contomap::editor
{

class Editor : public contomap::editor::InputRequestHandler
{
public:
   void helpWindowRequested() override;
};

} // namespace contomap::editor
