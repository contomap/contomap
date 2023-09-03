#pragma once

namespace contomap::frontend
{

class DisplayEnvironment
{
public:
   virtual ~DisplayEnvironment() = default;

   virtual void closeWindow() = 0;
};

} // namespace contomap::frontend
