#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include <raylib.h>
#include <raymath.h>
#pragma GCC diagnostic pop

#include "contomap/frontend/FocusInterceptor.h"
#include "contomap/frontend/Geometry.h"

using contomap::frontend::AssociationFocusItem;
using contomap::frontend::Focus;
using contomap::frontend::FocusInterceptor;
using contomap::frontend::OccurrenceFocusItem;
using contomap::frontend::RoleFocusItem;
using contomap::frontend::geometry::centerOf;
using contomap::model::Identifier;
using contomap::model::Style;

FocusInterceptor::FocusInterceptor(MapRenderer &next, Vector2 focusCoordinate)
   : next(next)
   , focusCoordinate(focusCoordinate)
{
}

Focus FocusInterceptor::getNewFocus() const
{
   return newFocus;
}

void FocusInterceptor::renderText(Rectangle area, Style const &style, std::string const &text, Font font, float fontSize, float spacing)
{
   next.renderText(area, style, text, font, fontSize, spacing);
}

void FocusInterceptor::renderOccurrencePlate(Identifier id, Rectangle area, Style const &style, Rectangle plate, float lineThickness, bool reified)
{
   if (CheckCollisionPointRec(focusCoordinate, area))
   {
      newFocus.registerItem(std::make_shared<OccurrenceFocusItem>(id), Vector2Distance(focusCoordinate, centerOf(area)));
   }
   next.renderOccurrencePlate(id, area, style, plate, lineThickness, reified);
}

void FocusInterceptor::renderAssociationPlate(Identifier id, Rectangle area, Style const &style, Rectangle plate, float lineThickness, bool reified)
{
   if (CheckCollisionPointRec(focusCoordinate, area))
   {
      newFocus.registerItem(std::make_shared<AssociationFocusItem>(id), Vector2Distance(focusCoordinate, centerOf(area)));
   }
   next.renderAssociationPlate(id, area, style, plate, lineThickness, reified);
}

void FocusInterceptor::renderRoleLine(Identifier id, Vector2 a, Vector2 b, Style const &style, float lineThickness, bool reified)
{
   if (CheckCollisionPointLine(focusCoordinate, a, b, 5))
   {
      newFocus.registerItem(std::make_shared<RoleFocusItem>(id), 0.0f);
   }
   next.renderRoleLine(id, a, b, style, lineThickness, reified);
}
