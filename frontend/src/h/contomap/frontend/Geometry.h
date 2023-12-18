#pragma once

#include <optional>
#include <tuple>

#include <raylib.h>

namespace contomap::frontend::geometry
{

/**
 * Calculate the point at which two lines intersect.
 *
 * @param a the start/end points of line A.
 * @param b the start/end points of line B.
 * @return the coordinate at which the two lines intersect, if they do.
 */
[[nodiscard]] std::optional<Vector2> intersectLines(std::tuple<Vector2, Vector2> a, std::tuple<Vector2, Vector2> b);

/**
 * Calculate the point at which a line intersects the border of a box.
 * One point of the line is at the center of a box.
 *
 * @param start the other point of the line.
 * @param box the box to intersect.
 * @return the point of the intersection, if the other point is outside the box.
 */
[[nodiscard]] std::optional<Vector2> intersectLineIntoBoxCenter(Vector2 start, Rectangle box);

/**
 * Calculate the center point of a rectangular area.
 *
 * @param area the area to consider.
 * @return the center point in that area.
 */
[[nodiscard]] Vector2 centerOf(Rectangle area);

}