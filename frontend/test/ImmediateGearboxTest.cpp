#include <gtest/gtest.h>

#include "contomap/frontend/MapCamera.h"

#include "contomap/test/Steps.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include <raymath.h>
#pragma GCC diagnostic pop

using contomap::frontend::MapCamera;

class ImmediateGearboxTest : public testing::Test, public contomap::test::Steps<ImmediateGearboxTest>
{
public:
   void SetUp() override
   {
      instance = std::make_unique<MapCamera::ImmediateGearbox>();
   }

   void timePasses(float seconds)
   {
      instance->timePassed(seconds);
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

   void currentZoomFactorShouldBe(MapCamera::ZoomFactor expected, std::string const &message = "")
   {
      EXPECT_EQ(expected, instance->getCurrentZoomFactor()) << message;
   }

   void targetZoomFactorShouldBe(MapCamera::ZoomFactor expected, std::string const &message = "")
   {
      EXPECT_EQ(expected, instance->getTargetZoomFactor()) << message;
   }

   static constexpr float oneSecond()
   {
      return 1.0f;
   }

   static constexpr Vector2 somePosition()
   {
      return Vector2 { .x = 10.0f, .y = 20.0f };
   }

private:
   std::unique_ptr<MapCamera::Gearbox> instance;
};

TEST_F(ImmediateGearboxTest, defaultStateAtConstruction)
{
   then().currentPositionShouldBeCloseTo(MapCamera::HOME_POSITION);
   asWellAs().currentZoomFactorShouldBe(MapCamera::ZoomFactor::UNIT);
   asWellAs().targetZoomFactorShouldBe(MapCamera::ZoomFactor::UNIT);
}

TEST_F(ImmediateGearboxTest, defaultStateAfterPassageOfTime)
{
   when().timePasses(oneSecond());
   then().currentPositionShouldBeCloseTo(MapCamera::HOME_POSITION);
   asWellAs().currentZoomFactorShouldBe(MapCamera::ZoomFactor::UNIT);
}

TEST_F(ImmediateGearboxTest, panToIsImmediate)
{
   Vector2 target = somePosition();
   when().panningTo(target);
   then().currentPositionShouldBeCloseTo(target);
}

TEST_F(ImmediateGearboxTest, panIsAppliedOverTime)
{
   float seconds = 4.0f;
   given().panning(true, false, false, true);
   when().timePasses(oneSecond() * seconds);
   then().currentPositionShouldBeCloseTo(Vector2Scale(Vector2Normalize(Vector2 { .x = -1.0f, .y = 1.0f }), MapCamera::PANNING_SPEED * seconds));
}

TEST_F(ImmediateGearboxTest, zoomFactorIsAppliedImmediately)
{
   auto factor = MapCamera::ZoomFactor::from(12.3f);
   when().zoomingTo(factor);
   then().currentZoomFactorShouldBe(factor);
   asWellAs().targetZoomFactorShouldBe(factor);
}
