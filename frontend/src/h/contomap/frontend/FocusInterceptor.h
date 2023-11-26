#pragma once

#include "contomap/frontend/Focus.h"
#include "contomap/frontend/MapRenderer.h"

namespace contomap::frontend
{

/**
 * FocusInterceptor intercepts render requests for map items to capture which one is under the focus.
 */
class FocusInterceptor : public contomap::frontend::MapRenderer
{
public:
   /**
    * Constructor.
    *
    * @param next the next renderer to call for each request.
    * @param focusCoordinate the coordinate to look at.
    */
   FocusInterceptor(contomap::frontend::MapRenderer &next, Vector2 focusCoordinate);
   ~FocusInterceptor() override = default;

   /**
    * @return the new, accumulated focus.
    */
   [[nodiscard]] Focus getNewFocus() const;

   void renderText(Rectangle area, contomap::model::Style const &style, std::string const &text, Font font, float fontSize, float spacing) override;
   void renderOccurrencePlate(
      contomap::model::Identifier id, Rectangle area, contomap::model::Style const &style, Rectangle plate, float lineThickness, bool reified) override;
   void renderAssociationPlate(
      contomap::model::Identifier id, Rectangle area, contomap::model::Style const &style, Rectangle plate, float lineThickness, bool reified) override;
   void renderRoleLine(contomap::model::Identifier id, Vector2 a, Vector2 b, contomap::model::Style const &style, float lineThickness, bool reified) override;

private:
   [[nodiscard]] static Vector2 centerOf(Rectangle area);

   contomap::frontend::MapRenderer &next;
   Vector2 focusCoordinate;
   Focus newFocus;
};

} // namespace contomap::frontend
