#pragma once

#include <string>

#include "contomap/frontend/MapRenderer.h"

namespace contomap::frontend
{

/**
 * DirectMapRenderer performs the actual render calls.
 */
class DirectMapRenderer : public contomap::frontend::MapRenderer
{
public:
   ~DirectMapRenderer() override = default;

   void renderText(Rectangle area, contomap::model::Style const &style, std::string const &text, Font font, float fontSize, float spacing) override;
   void renderOccurrencePlate(
      contomap::model::Identifier id, Rectangle area, contomap::model::Style const &style, Rectangle plate, float lineThickness, bool reified) override;
   void renderAssociationPlate(
      contomap::model::Identifier id, Rectangle area, contomap::model::Style const &style, Rectangle plate, float lineThickness, bool reified) override;
   void renderRoleLine(contomap::model::Identifier id, Vector2 a, Vector2 b, contomap::model::Style const &style, float lineThickness, bool reified) override;

private:
   static void drawLine(Vector2 a, Color colorA, Vector2 b, Color colorB, float thickness);
};

} // namespace contomap::frontend
