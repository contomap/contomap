#pragma once

namespace contomap::editor
{

class InputRequestHandler
{
public:
   virtual ~InputRequestHandler() = default;

   virtual void helpWindowShowRequested() = 0;
   virtual void helpWindowHideRequested() = 0;
};

} // namespace contomap::editor
