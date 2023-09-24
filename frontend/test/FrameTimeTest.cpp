#include <gtest/gtest.h>

#include "contomap/frontend/FrameTime.h"

using contomap::frontend::FrameTime;

TEST(FrameTimeTest, rawValueIsMaintained)
{
   auto raw = 123.456f;
   auto value = FrameTime::fromSeconds(raw);
   EXPECT_NEAR(raw, value.rawSeconds(), 0.001f);
}

TEST(FrameTimeTest, negativeValueIsClampedToZero)
{
   EXPECT_LT(FrameTime::fromSeconds(-0.01f).rawSeconds(), 0.001f);
   EXPECT_LT(FrameTime::fromSeconds(-1.0f).rawSeconds(), 0.001f);
   EXPECT_LT(FrameTime::fromSeconds(-100.0f).rawSeconds(), 0.001f);
}
