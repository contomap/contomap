#pragma once

namespace contomap::frontend
{

/**
 * DisplayEnvironment represents the context that the application is running in.
 * Any interaction towards this environment is made available here.
 */
class DisplayEnvironment
{
public:
   virtual ~DisplayEnvironment() = default;

   /**
    * Requests to close the window. This causes the frequent calls to draw the window to stop.
    */
   virtual void closeWindow() = 0;
};

} // namespace contomap::frontend
