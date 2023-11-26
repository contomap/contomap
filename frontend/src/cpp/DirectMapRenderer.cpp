#include <array>

#include <raylib.h>
#include <rlgl.h>

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
   float centerY = plate.y + (plate.height / 2.0f);
   {
      float halfHeight = plate.height / 2.0f;
      Vector2 center { .x = plate.x + (plate.width / 2.0f), .y = centerY };
      auto color = style.get(Style::ColorType::Fill);

      rlBegin(RL_TRIANGLES);
      rlColor4ub(color.red, color.green, color.blue, color.alpha);

      rlVertex2f(center.x, center.y);
      rlVertex2f(plate.x + plate.width, plate.y);
      rlVertex2f(plate.x, plate.y);

      rlVertex2f(center.x, center.y);
      rlVertex2f(plate.x + plate.width + halfHeight, centerY);
      rlVertex2f(plate.x + plate.width, plate.y);

      rlVertex2f(center.x, center.y);
      rlVertex2f(plate.x + plate.width, plate.y + plate.height);
      rlVertex2f(plate.x + plate.width + halfHeight, centerY);

      rlVertex2f(center.x, center.y);
      rlVertex2f(plate.x, plate.y + plate.height);
      rlVertex2f(plate.x + plate.width, plate.y + plate.height);

      rlVertex2f(center.x, center.y);
      rlVertex2f(plate.x - halfHeight, centerY);
      rlVertex2f(plate.x, plate.y + plate.height);

      rlVertex2f(center.x, center.y);
      rlVertex2f(plate.x, plate.y);
      rlVertex2f(plate.x - halfHeight, centerY);

      rlEnd();
   }
   {
      float halfHeight = plate.height / 2.0f;
      float slant = lineThickness * 0.4f;
      std::array<Vector2, 14> lineVertices {
         Vector2 { .x = plate.x, .y = plate.y - lineThickness },
         Vector2 { .x = plate.x + slant, .y = plate.y },
         Vector2 { .x = plate.x + plate.width, .y = plate.y - lineThickness },
         Vector2 { .x = plate.x + plate.width - slant, .y = plate.y },
         Vector2 { .x = plate.x + plate.width + halfHeight + lineThickness, .y = centerY },
         Vector2 { .x = plate.x + plate.width + halfHeight - slant, .y = centerY },
         Vector2 { .x = plate.x + plate.width, .y = plate.y + plate.height + lineThickness },
         Vector2 { .x = plate.x + plate.width - slant, .y = plate.y + plate.height },
         Vector2 { .x = plate.x, .y = plate.y + plate.height + lineThickness },
         Vector2 { .x = plate.x + slant, .y = plate.y + plate.height },
         Vector2 { .x = plate.x - halfHeight - lineThickness, .y = centerY },
         Vector2 { .x = plate.x - halfHeight + slant, .y = centerY },
         Vector2 { .x = plate.x, .y = plate.y - lineThickness },
         Vector2 { .x = plate.x + slant, .y = plate.y },
      };
      DrawTriangleStrip(lineVertices.data(), static_cast<int>(lineVertices.size()), Colors::toUiColor(style.get(Style::ColorType::Line)));
      if (reified)
      {
         std::array<Vector2, 6> leftVertices {
            Vector2 { .x = area.x + halfHeight + lineThickness, .y = area.y + area.height },
            Vector2 { .x = area.x + halfHeight + (lineThickness * 2.0f) + slant, .y = area.y + area.height },
            Vector2 { .x = area.x, .y = centerY },
            Vector2 { .x = area.x + lineThickness + slant, .y = centerY },
            Vector2 { .x = area.x + halfHeight + lineThickness, .y = area.y },
            Vector2 { .x = area.x + halfHeight + (lineThickness * 2.0f) + slant, .y = area.y },
         };
         std::array<Vector2, 6> rightVertices {
            Vector2 { .x = area.x + area.width - halfHeight - lineThickness, .y = area.y },
            Vector2 { .x = area.x + area.width - halfHeight - (lineThickness * 2.0f) - slant, .y = area.y },
            Vector2 { .x = area.x + area.width, .y = centerY },
            Vector2 { .x = area.x + area.width - lineThickness - slant, .y = centerY },
            Vector2 { .x = area.x + area.width - halfHeight - lineThickness, .y = area.y + area.height },
            Vector2 { .x = area.x + area.width - halfHeight - (lineThickness * 2.0f) - slant, .y = area.y + area.height },
         };
         DrawTriangleStrip(leftVertices.data(), static_cast<int>(leftVertices.size()), Colors::toUiColor(style.get(Style::ColorType::Line)));
         DrawTriangleStrip(rightVertices.data(), static_cast<int>(rightVertices.size()), Colors::toUiColor(style.get(Style::ColorType::Line)));
      }
   }
}

void DirectMapRenderer::renderRoleLine(Vector2 a, Vector2 b, contomap::model::Style const &style, float lineThickness, bool reified)
{
}
