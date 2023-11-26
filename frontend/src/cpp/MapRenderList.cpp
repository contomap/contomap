#include "contomap/frontend/MapRenderList.h"
#include "contomap/frontend/MapRenderer.h"

using contomap::frontend::MapRenderer;
using contomap::frontend::MapRenderList;
using contomap::model::Identifier;
using contomap::model::Style;

void MapRenderList::renderTo(contomap::frontend::MapRenderer &renderer) const
{
   for (auto const &command : commands)
   {
      command->renderTo(renderer);
   }
}

void MapRenderList::renderText(Rectangle area, Style const &style, std::string const &text, Font font, float fontSize, float spacing)
{
   commands.emplace_back(std::make_unique<TextRenderCommand>(area, style, text, font, fontSize, spacing));
}

void MapRenderList::renderOccurrencePlate(Identifier id, Rectangle area, Style const &style, Rectangle plate, float lineThickness, bool reified)
{
   commands.emplace_back(std::make_unique<OccurrenceRenderCommand>(id, area, style, plate, lineThickness, reified));
}

void MapRenderList::renderAssociationPlate(Identifier id, Rectangle area, Style const &style, Rectangle plate, float lineThickness, bool reified)
{
   commands.emplace_back(std::make_unique<AssociationRenderCommand>(id, area, style, plate, lineThickness, reified));
}

void MapRenderList::renderRoleLine(Identifier id, Vector2 a, Vector2 b, Style const &style, float lineThickness, bool reified)
{
   commands.emplace_back(std::make_unique<RoleRenderCommand>(id, a, b, style, lineThickness, reified));
}
