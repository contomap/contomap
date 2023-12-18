#include "contomap/frontend/MapRenderList.h"
#include "contomap/frontend/MapRenderer.h"

using contomap::frontend::MapRenderer;
using contomap::frontend::MapRenderList;
using contomap::model::Identifier;
using contomap::model::Style;

void MapRenderList::optimize()
{
   flushPendingCommand();
   commands.sort([](auto const &a, auto const &b) { return a->getSortLayer() < b->getSortLayer(); });
}

void MapRenderList::renderTo(contomap::frontend::MapRenderer &renderer) const
{
   for (auto const &command : commands)
   {
      command->renderTo(renderer);
   }
   if (pendingCommand != nullptr)
   {
      pendingCommand->renderTo(renderer);
   }
}

void MapRenderList::renderText(Rectangle area, Style const &style, std::string const &text, Font font, float fontSize, float spacing)
{
   addToLastCommand(std::make_unique<TextRenderCommand>(area, style, text, font, fontSize, spacing));
}

void MapRenderList::renderOccurrencePlate(Identifier id, Rectangle area, Style const &style, Rectangle plate, float lineThickness, bool reified)
{
   startNewCommand(std::make_unique<OccurrenceRenderCommand>(id, area, style, plate, lineThickness, reified));
}

void MapRenderList::renderAssociationPlate(Identifier id, Rectangle area, Style const &style, Rectangle plate, float lineThickness, bool reified)
{
   startNewCommand(std::make_unique<AssociationRenderCommand>(id, area, style, plate, lineThickness, reified));
}

void MapRenderList::renderRoleLine(Identifier id, Vector2 a, Vector2 b, Style const &style, float lineThickness, bool reified)
{
   startNewCommand(std::make_unique<RoleRenderCommand>(id, a, b, style, lineThickness, reified));
}

void MapRenderList::startNewCommand(std::unique_ptr<TypedRenderCommand> command)
{
   flushPendingCommand();
   pendingCommand = std::move(command);
}

void MapRenderList::addToLastCommand(std::unique_ptr<RenderCommand> command)
{
   if (pendingCommand == nullptr) [[unlikely]]
   {
      commands.emplace_back(std::make_unique<UnknownRenderCommand>(std::move(command)));
      return;
   }
   pendingCommand->addAuxiliary(std::move(command));
}

void MapRenderList::flushPendingCommand()
{
   if (pendingCommand != nullptr) [[likely]]
   {
      commands.emplace_back(std::move(pendingCommand));
   }
}
