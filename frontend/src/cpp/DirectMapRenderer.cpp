#include <array>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#pragma GCC diagnostic pop

#include "contomap/frontend/Colors.h"
#include "contomap/frontend/DirectMapRenderer.h"

using contomap::frontend::Colors;
using contomap::frontend::DirectMapRenderer;
using contomap::model::Identifier;
using contomap::model::Style;

void DirectMapRenderer::renderText(Rectangle area, Style const &style, std::string const &text, Font font, float fontSize, float spacing)
{
   DrawRectangleRec(area, Colors::toUiColor(style.get(Style::ColorType::Fill)));
   DrawTextEx(font, text.c_str(), Vector2 { .x = area.x, .y = area.y }, fontSize, spacing, Colors::toUiColor(style.get(Style::ColorType::Text)));
}

void DirectMapRenderer::renderOccurrencePlate(Identifier, Rectangle area, Style const &style, Rectangle plate, float lineThickness, bool reified)
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

void DirectMapRenderer::renderAssociationPlate(Identifier, Rectangle area, Style const &style, Rectangle plate, float lineThickness, bool reified)
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

void DirectMapRenderer::renderRoleLine(Identifier, Vector2 a, Vector2 b, Style const &style, float lineThickness, bool reified)
{
   auto color = Colors::toUiColor(style.get(Style::ColorType::Line));
   auto drawLineShadedEnds = [lineThickness, color](Vector2 a, Vector2 b) {
      float const shadeLength = 7.5f;
      Vector2 centerPoint { .x = (b.x + a.x) / 2.0f, .y = (b.y + a.y) / 2.0f };
      Vector2 centerToA = Vector2Subtract(a, centerPoint);
      float halfLength = Vector2Length(centerToA);
      Vector2 centerToShadeA { .x = 0.0f, .y = 0.0f };
      uint8_t shadeAlpha = 0xFF;
      if (halfLength >= shadeLength)
      {
         centerToShadeA = Vector2Scale(centerToA, (halfLength - shadeLength) / halfLength);
      }
      else
      {
         shadeAlpha = static_cast<uint8_t>(std::roundf((255.0f * halfLength) / shadeLength));
      }
      Vector2 shadeAPoint = Vector2Add(centerPoint, centerToShadeA);
      Vector2 shadeBPoint = Vector2Subtract(centerPoint, centerToShadeA);
      Color endColor { .r = color.r, .g = color.g, .b = color.b, .a = 0x00 };
      Color shadeColor { .r = color.r, .g = color.g, .b = color.b, .a = shadeAlpha };

      drawLine(Vector2Add(centerPoint, centerToA), endColor, shadeAPoint, shadeColor, lineThickness);
      drawLine(Vector2Subtract(centerPoint, centerToA), endColor, shadeBPoint, shadeColor, lineThickness);
      drawLine(shadeAPoint, shadeColor, shadeBPoint, shadeColor, lineThickness);
   };
   drawLineShadedEnds(a, b);
   float diffX = a.x - b.x;
   float diffY = a.y - b.y;
   float length = Vector2Length(Vector2 { .x = diffX, .y = diffY });
   if ((length > 0.0001f) && reified)
   {
      auto drawWithOffset = [&drawLineShadedEnds, length, a, b](float offset) {
         Vector2 shiftedA {
            .x = a.x + offset * (b.y - a.y) / length,
            .y = a.y + offset * (a.x - b.x) / length,
         };
         Vector2 shiftedB {
            .x = b.x + offset * (b.y - a.y) / length,
            .y = b.y + offset * (a.x - b.x) / length,
         };
         float diffX = shiftedB.x - shiftedA.x;
         float diffY = shiftedB.y - shiftedA.y;
         shiftedA.x += diffX / 3;
         shiftedA.y += diffY / 3;
         shiftedB.x += -diffX / 3;
         shiftedB.y += -diffY / 3;
         drawLineShadedEnds(shiftedA, shiftedB);
      };

      drawWithOffset(+3.0f);
      drawWithOffset(-3.0f);
   }
}

void DirectMapRenderer::drawLine(Vector2 a, Color colorA, Vector2 b, Color colorB, float thickness)
{
   Vector2 delta = { b.x - a.x, b.y - a.y };
   float length = sqrtf(delta.x * delta.x + delta.y * delta.y);
   if ((length > 0) && (thickness > 0))
   {
      float scale = thickness / (2 * length);
      Vector2 radius = { -scale * delta.y, scale * delta.x };
      std::array<Vector2, 4> strip {
         Vector2 { a.x - radius.x, a.y - radius.y },
         Vector2 { a.x + radius.x, a.y + radius.y },
         Vector2 { b.x - radius.x, b.y - radius.y },
         Vector2 { b.x + radius.x, b.y + radius.y },
      };

      rlBegin(RL_TRIANGLES);
      rlColor4ub(colorA.r, colorA.g, colorA.b, colorA.a);
      rlVertex2f(strip[0].x, strip[0].y);
      rlVertex2f(strip[1].x, strip[1].y);
      rlColor4ub(colorB.r, colorB.g, colorB.b, colorB.a);
      rlVertex2f(strip[2].x, strip[2].y);

      rlVertex2f(strip[2].x, strip[2].y);
      rlColor4ub(colorA.r, colorA.g, colorA.b, colorA.a);
      rlVertex2f(strip[1].x, strip[1].y);
      rlColor4ub(colorB.r, colorB.g, colorB.b, colorB.a);
      rlVertex2f(strip[3].x, strip[3].y);
      rlEnd();
   }
}
