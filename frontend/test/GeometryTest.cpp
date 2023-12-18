#include <gmock/gmock.h>

#include "contomap/frontend/Geometry.h"

using contomap::frontend::geometry::centerOf;
using contomap::frontend::geometry::intersectLineIntoBoxCenter;
using contomap::frontend::geometry::intersectLines;

MATCHER_P(isCloseTo, expected, "")
{
   return (std::abs(arg.x - expected.x) < 0.001f) && (std::abs(arg.y - expected.y) < 0.001f);
}

std::ostream &operator<<(std::ostream &os, Vector2 const &v)
{
   return os << ".x=" << v.x << "f, .y=" << v.y << "f";
}

TEST(GeometryTest, centerOfRectangle)
{
   EXPECT_THAT(centerOf(Rectangle { .x = 0.0f, .y = 0.0f, .width = 0.0f, .height = 0.0f }), isCloseTo(Vector2 { .x = 0.0f, .y = 0.0f }));
   EXPECT_THAT(centerOf(Rectangle { .x = 0.0f, .y = 0.0f, .width = 10.0f, .height = 20.0f }), isCloseTo(Vector2 { .x = 5.0f, .y = 10.0f }));
   EXPECT_THAT(centerOf(Rectangle { .x = -5.0f, .y = -10.0f, .width = 10.0f, .height = 20.0f }), isCloseTo(Vector2 { .x = 0.0f, .y = 0.0f }));
}

TEST(GeometryTest, intersectLines)
{
   auto line1 = std::make_tuple(Vector2 { .x = 1.0f, .y = 1.0f }, Vector2 { .x = 10.0f, .y = 1.0f });
   auto line2 = std::make_tuple(Vector2 { .x = 1.0f, .y = 2.0f }, Vector2 { .x = 10.0f, .y = 2.0f });
   auto line3 = std::make_tuple(Vector2 { .x = 2.0f, .y = 0.5f }, Vector2 { .x = 2.0f, .y = 2.0f });
   auto zeroX = std::make_tuple(Vector2 { .x = 0.0f, .y = -1.0f }, Vector2 { .x = 0.0f, .y = 1.0f });
   auto zeroY = std::make_tuple(Vector2 { .x = -1.0f, .y = 0.0f }, Vector2 { .x = 1.0f, .y = 0.0f });
   auto pointOn1 = std::make_tuple(Vector2 { .x = 3.0f, .y = 1.0f }, Vector2 { .x = 3.0f, .y = 1.0f });

   EXPECT_FALSE(intersectLines(line1, line2).has_value()) << "non-crossing lines";
   EXPECT_THAT(intersectLines(line1, line3).value(), isCloseTo(Vector2 { .x = 2.0f, .y = 1.0f })) << "crossing lines";
   EXPECT_THAT(intersectLines(line2, line3).value(), isCloseTo(Vector2 { .x = 2.0f, .y = 2.0f })) << "touching line";

   EXPECT_FALSE(intersectLines(line1, line1).has_value()) << "overlapping line";

   EXPECT_THAT(intersectLines(zeroX, zeroY).value(), isCloseTo(Vector2 { .x = 0.0f, .y = 0.0f })) << "lines with zero component";

   EXPECT_FALSE(intersectLines(line2, pointOn1).has_value()) << "point outside line";
   EXPECT_FALSE(intersectLines(line1, pointOn1).has_value()) << "point on line"; // accepted limitation of implementation
}

TEST(GeometryTest, intersectLineIntoBoxCenter)
{
   Rectangle box { .x = 1.0f, .y = 1.0f, .width = 1.0f, .height = 1.0f };
   EXPECT_FALSE(intersectLineIntoBoxCenter(Vector2 { .x = 1.5f, .y = 1.5f }, box).has_value()) << "start equal center";
   EXPECT_FALSE(intersectLineIntoBoxCenter(Vector2 { .x = 1.25f, .y = 1.25f }, box).has_value()) << "start within box";

   EXPECT_THAT(intersectLineIntoBoxCenter(Vector2 { .x = 0.0f, .y = 1.5f }, box).value(), isCloseTo(Vector2 { .x = 1.0f, .y = 1.5f })) << "at left side";
   EXPECT_THAT(intersectLineIntoBoxCenter(Vector2 { .x = 1.5f, .y = 0.0f }, box).value(), isCloseTo(Vector2 { .x = 1.5f, .y = 1.0f })) << "at bottom side";
   EXPECT_THAT(intersectLineIntoBoxCenter(Vector2 { .x = 1.5f, .y = 3.0f }, box).value(), isCloseTo(Vector2 { .x = 1.5f, .y = 2.0f })) << "at top side";
   EXPECT_THAT(intersectLineIntoBoxCenter(Vector2 { .x = 3.0f, .y = 1.5f }, box).value(), isCloseTo(Vector2 { .x = 2.0f, .y = 1.5f })) << "at right side";

   EXPECT_THAT(intersectLineIntoBoxCenter(Vector2 { .x = 0.0f, .y = 0.0f }, box).value(), isCloseTo(Vector2 { .x = 1.0f, .y = 1.0f })) << "at corner";
}
