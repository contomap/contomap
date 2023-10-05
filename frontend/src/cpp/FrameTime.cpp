#include <algorithm>

#include <raylib.h>

#include "contomap/frontend/FrameTime.h"

using contomap::frontend::FrameTime;

FrameTime::FrameTime(float seconds)
   : seconds(seconds)
{
}

FrameTime FrameTime::fromSeconds(float raw)
{
   return FrameTime(std::max(raw, 0.0f));
}

FrameTime FrameTime::fromLastFrame()
{
   return FrameTime::fromSeconds(GetFrameTime());
}

float FrameTime::rawSeconds() const
{
   return seconds;
}
