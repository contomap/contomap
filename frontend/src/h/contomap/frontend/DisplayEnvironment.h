#pragma once

namespace contomap::frontend
{

class DisplayEnvironment
{
public:
   virtual ~DisplayEnvironment() = default;

   virtual void closeWindow() = 0;
   virtual bool isFullscreen() = 0;
   virtual void toggleFullscreen() = 0;
};

} // namespace contomap::frontend
