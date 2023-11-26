#pragma once

#include <list>
#include <utility>

#include "contomap/frontend/Focus.h"
#include "contomap/frontend/MapRenderer.h"

namespace contomap::frontend
{

/**
 * MapRenderList stores all render calls in an internal list of commands.
 */
class MapRenderList : public contomap::frontend::MapRenderer
{
public:
   ~MapRenderList() override = default;

   /**
    * Requests to render the list to the given renderer.
    *
    * @param renderer the renderer to call
    */
   void renderTo(contomap::frontend::MapRenderer &renderer) const;

   void renderText(Rectangle area, contomap::model::Style const &style, std::string const &text, Font font, float fontSize, float spacing) override;
   void renderOccurrencePlate(
      contomap::model::Identifier id, Rectangle area, contomap::model::Style const &style, Rectangle plate, float lineThickness, bool reified) override;
   void renderAssociationPlate(
      contomap::model::Identifier id, Rectangle area, contomap::model::Style const &style, Rectangle plate, float lineThickness, bool reified) override;
   void renderRoleLine(contomap::model::Identifier id, Vector2 a, Vector2 b, contomap::model::Style const &style, float lineThickness, bool reified) override;

private:
   class RenderCommand
   {
   public:
      virtual ~RenderCommand() = default;

      virtual void renderTo(contomap::frontend::MapRenderer &renderer) const = 0;
   };

   class TextRenderCommand : public RenderCommand
   {
   public:
      TextRenderCommand(Rectangle area, contomap::model::Style style, std::string text, Font font, float fontSize, float spacing)
         : area(area)
         , style(std::move(style))
         , text(std::move(text))
         , font(font)
         , fontSize(fontSize)
         , spacing(spacing)
      {
      }

      void renderTo(contomap::frontend::MapRenderer &renderer) const override
      {
         renderer.renderText(area, style, text, font, fontSize, spacing);
      }

   private:
      Rectangle area;
      contomap::model::Style style;
      std::string text;
      Font font;
      float fontSize;
      float spacing;
   };

   class OccurrenceRenderCommand : public RenderCommand
   {
   public:
      OccurrenceRenderCommand(contomap::model::Identifier id, Rectangle area, contomap::model::Style style, Rectangle plate, float lineThickness, bool reified)
         : id(id)
         , area(area)
         , style(std::move(style))
         , plate(plate)
         , lineThickness(lineThickness)
         , reified(reified)
      {
      }

      void renderTo(contomap::frontend::MapRenderer &renderer) const override
      {
         renderer.renderOccurrencePlate(id, area, style, plate, lineThickness, reified);
      }

   private:
      contomap::model::Identifier id;
      Rectangle area;
      contomap::model::Style style;
      Rectangle plate;
      float lineThickness;
      bool reified;
   };

   class AssociationRenderCommand : public RenderCommand
   {
   public:
      AssociationRenderCommand(contomap::model::Identifier id, Rectangle area, contomap::model::Style style, Rectangle plate, float lineThickness, bool reified)
         : id(id)
         , area(area)
         , style(std::move(style))
         , plate(plate)
         , lineThickness(lineThickness)
         , reified(reified)
      {
      }

      void renderTo(contomap::frontend::MapRenderer &renderer) const override
      {
         renderer.renderAssociationPlate(id, area, style, plate, lineThickness, reified);
      }

   private:
      contomap::model::Identifier id;
      Rectangle area;
      contomap::model::Style style;
      Rectangle plate;
      float lineThickness;
      bool reified;
   };

   class RoleRenderCommand : public RenderCommand
   {
   public:
      RoleRenderCommand(contomap::model::Identifier id, Vector2 a, Vector2 b, contomap::model::Style style, float lineThickness, bool reified)
         : id(id)
         , a(a)
         , b(b)
         , style(std::move(style))
         , lineThickness(lineThickness)
         , reified(reified)
      {
      }

      void renderTo(contomap::frontend::MapRenderer &renderer) const override
      {
         renderer.renderRoleLine(id, a, b, style, lineThickness, reified);
      }

   private:
      contomap::model::Identifier id;
      Vector2 a;
      Vector2 b;
      contomap::model::Style style;
      float lineThickness;
      bool reified;
   };

   std::list<std::unique_ptr<RenderCommand>> commands;
};

} // namespace contomap::frontend
