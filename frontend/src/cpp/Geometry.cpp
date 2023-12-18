#include <algorithm>
#include <array>

#include <raylib.h>

#include "contomap/frontend/Geometry.h"

std::optional<Vector2> contomap::frontend::geometry::intersectLines(std::tuple<Vector2, Vector2> a, std::tuple<Vector2, Vector2> b)
{
   auto [a1, a2] = a;
   auto [b1, b2] = b;
   float diff = (b2.y - b1.y) * (a2.x - a1.x) - (b2.x - b1.x) * (a2.y - a1.y);
   if (std::abs(diff) < 0.000001f) [[unlikely]]
   {
      return {};
   }
   float uA = ((b2.x - b1.x) * (a1.y - b1.y) - (b2.y - b1.y) * (a1.x - b1.x)) / diff;
   float uB = ((a2.x - a1.x) * (a1.y - b1.y) - (a2.y - a1.y) * (a1.x - b1.x)) / diff;

   if ((uA >= 0.0f) && (uA <= 1.0f) && (uB >= 0.0f) && (uB <= 1.0f))
   {
      return { Vector2 { .x = a1.x + (uA * (a2.x - a1.x)), .y = a1.y + (uA * (a2.y - a1.y)) } };
   }
   return {};
}

std::optional<Vector2> contomap::frontend::geometry::intersectLineIntoBoxCenter(Vector2 start, Rectangle box)
{
   auto line = std::make_tuple(start, contomap::frontend::geometry::centerOf(box));
   Vector2 bl { .x = box.x, .y = box.y };
   Vector2 tl { .x = box.x, .y = box.y + box.height };
   Vector2 tr { .x = box.x + box.width, .y = box.y + box.height };
   Vector2 br { .x = box.x + box.width, .y = box.y };
   std::array<std::optional<Vector2>, 4> matches {
      intersectLines(line, std::make_tuple(bl, tl)),
      intersectLines(line, std::make_tuple(tl, tr)),
      intersectLines(line, std::make_tuple(tr, br)),
      intersectLines(line, std::make_tuple(br, bl)),
   };
   auto found = std::find_if(matches.begin(), matches.end(), [](auto const &entry) { return entry.has_value(); });
   if (found != matches.end()) [[likely]]
   {
      return *found;
   }
   return {};
}

Vector2 contomap::frontend::geometry::centerOf(Rectangle area)
{
   return Vector2 { .x = area.x + (area.width / 2.0f), .y = area.y + (area.height / 2.0f) };
}
