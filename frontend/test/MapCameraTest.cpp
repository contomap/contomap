#include <gmock/gmock.h>

#include "contomap/frontend/MapCamera.h"

using contomap::frontend::MapCamera;

TEST(MapCameraZoomLevelTest, comparison)
{
   EXPECT_EQ(MapCamera::ZoomFactor::NEAR_LIMIT, MapCamera::ZoomFactor::NEAR_LIMIT);
   EXPECT_GT(MapCamera::ZoomFactor::NEAR_LIMIT, MapCamera::ZoomFactor::FAR_LIMIT);
   EXPECT_NEAR(1.0f, MapCamera::ZoomFactor::UNIT.raw(), 0.0000001f);

   EXPECT_GT(MapCamera::ZoomFactor::from(1.01f), MapCamera::ZoomFactor::UNIT);
   EXPECT_LT(MapCamera::ZoomFactor::from(0.99f), MapCamera::ZoomFactor::UNIT);

   EXPECT_NEAR(0.1234f, MapCamera::ZoomFactor::from(0.1234f).raw(), 0.0001f);
}

TEST(MapCameraZoomLevelTest, limits)
{
   EXPECT_NEAR(MapCamera::ZoomFactor::FAR_LIMIT.raw(), MapCamera::ZoomFactor::from(std::numeric_limits<float>::lowest()).raw(), 0.0000001f);
   EXPECT_NEAR(MapCamera::ZoomFactor::FAR_LIMIT.raw(), MapCamera::ZoomFactor::from(0.0f).raw(), 0.0000001f);
   EXPECT_NEAR(MapCamera::ZoomFactor::NEAR_LIMIT.raw(), MapCamera::ZoomFactor::from(std::numeric_limits<float>::max()).raw(), 0.0000001f);
}