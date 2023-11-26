#pragma once

#include "contomap/frontend/Focus.h"
#include "contomap/frontend/MapRenderer.h"

namespace contomap::frontend
{

/**
 * MapRenderMeasurer determines the overall size of a map.
 */
class MapRenderMeasurer : public contomap::frontend::MapRenderer
{
public:
   /**
    * Constructor.
    */
   MapRenderMeasurer();
   ~MapRenderMeasurer() override = default;

   /**
    * @return the measured area.
    */
   [[nodiscard]] Rectangle getArea() const;

   void renderText(Rectangle area, contomap::model::Style const &style, std::string const &text, Font font, float fontSize, float spacing) override;
   void renderOccurrencePlate(
      contomap::model::Identifier id, Rectangle area, contomap::model::Style const &style, Rectangle plate, float lineThickness, bool reified) override;
   void renderAssociationPlate(
      contomap::model::Identifier id, Rectangle area, contomap::model::Style const &style, Rectangle plate, float lineThickness, bool reified) override;
   void renderRoleLine(contomap::model::Identifier id, Vector2 a, Vector2 b, contomap::model::Style const &style, float lineThickness, bool reified) override;

private:
   void addArea(Rectangle area);
   void addPoint(Vector2 point);

   Vector2 minPoint;
   Vector2 maxPoint;
};

} // namespace contomap::frontend
