#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include <raylib.h>
#include <raymath.h>
#pragma GCC diagnostic pop

#include "contomap/frontend/MapRenderMeasurer.h"

using contomap::frontend::MapRenderMeasurer;
using contomap::model::Identifier;
using contomap::model::Style;

MapRenderMeasurer::MapRenderMeasurer()
   : minPoint { .x = 0.0f, .y = 0.0f }
   , maxPoint { .x = 0.0f, .y = 0.0f }
{
}

Rectangle MapRenderMeasurer::getArea() const
{
   return Rectangle { .x = minPoint.x, .y = minPoint.y, .width = maxPoint.x - minPoint.x, .height = maxPoint.y - minPoint.y };
}

void MapRenderMeasurer::renderText(Rectangle area, Style const &, std::string const &, Font, float, float)
{
   addArea(area);
}

void MapRenderMeasurer::renderOccurrencePlate(Identifier, Rectangle area, Style const &, Rectangle, float, bool)
{
   addArea(area);
}

void MapRenderMeasurer::renderAssociationPlate(Identifier, Rectangle area, Style const &, Rectangle, float, bool)
{
   addArea(area);
}

void MapRenderMeasurer::renderRoleLine(Identifier, Vector2 a, Vector2 b, Style const &, float, bool)
{
   addPoint(a);
   addPoint(b);
}

void MapRenderMeasurer::addArea(Rectangle area)
{
   addPoint(Vector2 { .x = area.x, .y = area.y });
   addPoint(Vector2 { .x = area.x + area.width, .y = area.y + area.height });
}

void MapRenderMeasurer::addPoint(Vector2 point)
{
   if (point.x < minPoint.x)
   {
      minPoint.x = point.x;
   }
   else if (point.x > maxPoint.x)
   {
      maxPoint.x = point.x;
   }

   if (point.y < minPoint.y)
   {
      minPoint.y = point.y;
   }
   else if (point.y > maxPoint.y)
   {
      maxPoint.y = point.y;
   }
}
