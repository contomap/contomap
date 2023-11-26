#include <array>

#include "contomap/frontend/Colors.h"
#include "contomap/frontend/DirectMapRenderer.h"

using contomap::frontend::Colors;
using contomap::frontend::DirectMapRenderer;
using contomap::model::Style;

void DirectMapRenderer::renderText(Rectangle area, contomap::model::Style const &style, std::string const &text, Font font, float fontSize, float spacing)
{
   DrawRectangleRec(area, Colors::toUiColor(style.get(Style::ColorType::Fill)));
   DrawTextEx(font, text.c_str(), Vector2 { .x = area.x, .y = area.y }, fontSize, spacing, Colors::toUiColor(style.get(Style::ColorType::Text)));
}

void DirectMapRenderer::renderOccurrencePlate(Rectangle area, contomap::model::Style const &style, Rectangle plate, float lineThickness, bool reified)
{
   DrawRectangleRec(plate, Colors::toUiColor(style.get(Style::ColorType::Fill)));
   std::array<Vector2, 10> vertices {
      Vector2 { .x = plate.x - lineThickness, .y = plate.y - lineThickness },
      Vector2 { .x = plate.x, .y = plate.y },
      Vector2 { .x = plate.x + plate.width + lineThickness, .y = plate.y - lineThickness },
      Vector2 { .x = plate.x + plate.width, .y = plate.y },
      Vector2 { .x = plate.x + plate.width + lineThickness, .y = plate.y + plate.height + lineThickness },
      Vector2 { .x = plate.x + plate.width, .y = plate.y + plate.height },
      Vector2 { .x = plate.x - lineThickness, .y = plate.y + plate.height + lineThickness },
      Vector2 { .x = plate.x, .y = plate.y + plate.height },
      Vector2 { .x = plate.x - lineThickness, .y = plate.y - lineThickness },
      Vector2 { .x = plate.x, .y = plate.y },
   };
   auto lineColor = Colors::toUiColor(style.get(Style::ColorType::Line));
   DrawTriangleStrip(vertices.data(), static_cast<int>(vertices.size()), lineColor);
   if (reified)
   {
      float half = lineThickness / 2.0f;
      DrawLineEx(Vector2 { .x = area.x + half, .y = area.y }, Vector2 { .x = area.x + half, .y = area.y + area.height }, lineThickness, lineColor);
      DrawLineEx(Vector2 { .x = area.x + area.width - half, .y = area.y }, Vector2 { .x = area.x + area.width - half, .y = area.y + area.height },
         lineThickness, lineColor);
   }
}

void DirectMapRenderer::renderAssociationPlate(Rectangle area, contomap::model::Style const &style, Rectangle plate, float lineThickness, bool reified)
{
}

void DirectMapRenderer::renderRoleLine(Vector2 a, Vector2 b, contomap::model::Style const &style, float lineThickness, bool reified)
{
}
