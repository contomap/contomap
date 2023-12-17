#include <gtest/gtest.h>

#include "contomap/frontend/MapCamera.h"

#include "contomap/test/Steps.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include <raymath.h>
#pragma GCC diagnostic pop

using contomap::frontend::FrameTime;
using contomap::frontend::MapCamera;

class SmoothGearboxTest : public testing::Test, public contomap::test::Steps<SmoothGearboxTest>
{
public:
   void SetUp() override
   {
      instance = std::make_unique<MapCamera::SmoothGearbox>();
   }

   void timePasses(FrameTime amount)
   {
      instance->timePassed(amount);
   }

   void panningTo(Vector2 target)
   {
      instance->panTo(target);
   }

   void panning(bool left, bool up, bool right, bool down)
   {
      instance->pan(left, up, right, down);
   }

   void zoomingTo(MapCamera::ZoomFactor factor)
   {
      instance->setTargetZoomFactor(factor);
   }

   void currentPositionShouldBeCloseTo(Vector2 expected, std::string const &message = "")
   {
      auto current = instance->getCurrentPosition();
      EXPECT_LT(Vector2Distance(expected, current), 0.001f) << "current: " << current.x << ":" << current.y << " - " << message;
   }

   void targetPositionShouldBeCloseTo(Vector2 expected, std::string const &message = "")
   {
      auto current = instance->getTargetPosition();
      EXPECT_LT(Vector2Distance(expected, current), 0.001f) << "target: " << current.x << ":" << current.y << " - " << message;
   }

   void currentZoomFactorShouldBeNear(MapCamera::ZoomFactor expected, std::string const &message = "")
   {
      EXPECT_NEAR(expected.raw(), instance->getCurrentZoomFactor().raw(), 0.001f) << message;
   }

   void targetZoomFactorShouldBeNear(MapCamera::ZoomFactor expected, std::string const &message = "")
   {
      EXPECT_NEAR(expected.raw(), instance->getTargetZoomFactor().raw(), 0.001f) << message;
   }

   static FrameTime oneSecond()
   {
      return ofSeconds(1.0f);
   }

   static FrameTime ofSeconds(float value)
   {
      return FrameTime::fromSeconds(value);
   }

   static constexpr Vector2 somePosition()
   {
      return Vector2 { .x = 10.0f, .y = 20.0f };
   }

private:
   std::unique_ptr<MapCamera::Gearbox> instance;
};

TEST_F(SmoothGearboxTest, defaultStateAtConstruction)
{
   then().currentPositionShouldBeCloseTo(MapCamera::HOME_POSITION);
   asWellAs().targetPositionShouldBeCloseTo(MapCamera::HOME_POSITION);
   asWellAs().currentZoomFactorShouldBeNear(MapCamera::ZoomFactor::UNIT);
   asWellAs().targetZoomFactorShouldBeNear(MapCamera::ZoomFactor::UNIT);
}

TEST_F(SmoothGearboxTest, defaultStateAfterPassageOfTime)
{
   when().timePasses(oneSecond());
   then().currentPositionShouldBeCloseTo(MapCamera::HOME_POSITION);
   asWellAs().currentZoomFactorShouldBeNear(MapCamera::ZoomFactor::UNIT);
}

TEST_F(SmoothGearboxTest, panToIsSmooth)
{
   Vector2 target { .x = 10.0f, .y = 20.0f };
   given().panningTo(target);
   when().timePasses(ofSeconds(0.01f));
   then().currentPositionShouldBeCloseTo(Vector2 { .x = 1.333f, .y = 2.666f });
}

TEST_F(SmoothGearboxTest, panIsAppliedOverTime)
{
   float seconds = 4.0f;
   given().panning(true, false, false, true);
   when().timePasses(ofSeconds(seconds));
   then().currentPositionShouldBeCloseTo(Vector2Scale(Vector2Normalize(Vector2 { .x = -1.0f, .y = 1.0f }), MapCamera::PANNING_SPEED * seconds));
}

TEST_F(SmoothGearboxTest, zoomFactorIsAppliedSmoothly)
{
   auto factor = MapCamera::ZoomFactor::from(12.3f);
   given().zoomingTo(factor);
   when().timePasses(ofSeconds(0.01f));
   then().currentZoomFactorShouldBeNear(MapCamera::ZoomFactor::from(2.5066f));
   asWellAs().targetZoomFactorShouldBeNear(factor);
}
