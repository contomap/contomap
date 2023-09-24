#pragma once

#include <cstdint>

namespace contomap::frontend
{

/**
 * FrameTime represents the duration of a (rendered) frame.
 * The value is expressed as a non-negative floating point value in unit of seconds.
 */
class FrameTime
{
public:
   /**
    * Create a new instance from given raw value.
    * While negative values are invalid, the function clamps such input to zero to simplify user code.
    *
    * @param raw the raw value, in seconds.
    * @return the created instance.
    */
   static FrameTime fromSeconds(float raw);

   /**
    * Create a new instance based on previous render time.
    *
    * @return the created instance.
    */
   static FrameTime fromLastFrame();

   /**
    * @return the raw value of the frame time, in seconds.
    */
   float rawSeconds() const;

private:
   explicit FrameTime(float seconds);

   float seconds;
};

} // namespace contomap::frontend
