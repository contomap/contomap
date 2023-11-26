#pragma once

#include <string>

#include <raylib.h>

#include "contomap/model/Style.h"

namespace contomap::frontend
{

/**
 * MapRenderer provides primitives for rendering a map.
 */
class MapRenderer
{
public:
   virtual ~MapRenderer() = default;

   /**
    * Render a text block with background - depending on the style.
    *
    * @param area the area within which the text shall be drawn.
    * @param style the style in which the text is drawn.
    * @param text the string to render.
    * @param font the font to use.
    * @param fontSize the font size to use.
    * @param spacing the spacing to use.
    */
   virtual void renderText(Rectangle area, contomap::model::Style const &style, std::string const &text, Font font, float fontSize, float spacing) = 0;

   /**
    * Render an occurrence plate.
    *
    * @param area the overall area within which the plate is put.
    * @param style the style to use for drawing.
    * @param plate the area of the central plate.
    * @param lineThickness the thickness of lines.
    * @param reified whether the reification state should be drawn.
    */
   virtual void renderOccurrencePlate(Rectangle area, contomap::model::Style const &style, Rectangle plate, float lineThickness, bool reified) = 0;

   /**
    * Render an association plate.
    *
    * @param area the overall area within which the plate is put.
    * @param style the style to use for drawing.
    * @param plate the area of the central plate.
    * @param lineThickness the thickness of lines.
    * @param reified whether the reification state should be drawn.
    */
   virtual void renderAssociationPlate(Rectangle area, contomap::model::Style const &style, Rectangle plate, float lineThickness, bool reified) = 0;

   /**
    * Render the line of a role.
    *
    * @param a first position.
    * @param b second position.
    * @param style the style to use for drawing.
    * @param lineThickness the thickness of lines.
    * @param reified whether the reification state should be drawn.
    */
   virtual void renderRoleLine(Vector2 a, Vector2 b, contomap::model::Style const &style, float lineThickness, bool reified) = 0;
};

} // namespace contomap::frontend
