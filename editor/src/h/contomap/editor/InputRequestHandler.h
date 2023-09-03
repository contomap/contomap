#pragma once

namespace contomap::editor
{

class InputRequestHandler
{
public:
   virtual ~InputRequestHandler() = default;

   virtual void helpWindowRequested() = 0;
};

} // namespace contomap::editor
