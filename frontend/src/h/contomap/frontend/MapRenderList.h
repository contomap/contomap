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
    * Optimizes the list for better display.
    */
   void optimize();

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

   class RenderCommandStack : public RenderCommand
   {
   public:
      void add(std::unique_ptr<RenderCommand> command)
      {
         commands.emplace_back(std::move(command));
      }

      void renderTo(contomap::frontend::MapRenderer &renderer) const override
      {
         for (auto const &command : commands)
         {
            command->renderTo(renderer);
         }
      }

   private:
      std::list<std::unique_ptr<RenderCommand>> commands;
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

   class TypedRenderCommand : public RenderCommand
   {
   public:
      enum class SortLayer
      {
         Roles = 0,
         Associations = 1,
         Occurrences = 2,
         Unknown = 3,
      };

      void renderTo(contomap::frontend::MapRenderer &renderer) const override
      {
         renderBaseTo(renderer);
         if (auxiliaryCommands != nullptr)
         {
            auxiliaryCommands->renderTo(renderer);
         }
      }

      void addAuxiliary(std::unique_ptr<RenderCommand> command)
      {
         if (auxiliaryCommands == nullptr)
         {
            auxiliaryCommands = std::make_unique<RenderCommandStack>();
         }
         auxiliaryCommands->add(std::move(command));
      }

      [[nodiscard]] virtual SortLayer getSortLayer() const = 0;

   protected:
      virtual void renderBaseTo(contomap::frontend::MapRenderer &renderer) const = 0;

   private:
      std::unique_ptr<RenderCommandStack> auxiliaryCommands;
   };

   class UnknownRenderCommand : public TypedRenderCommand
   {
   public:
      explicit UnknownRenderCommand(std::unique_ptr<RenderCommand> nested)
         : nested(std::move(nested))
      {
      }

      [[nodiscard]] SortLayer getSortLayer() const override
      {
         return SortLayer::Unknown;
      }

      void renderBaseTo(contomap::frontend::MapRenderer &renderer) const override
      {
         nested->renderTo(renderer);
      }

   private:
      std::unique_ptr<RenderCommand> nested;
   };

   class OccurrenceRenderCommand : public TypedRenderCommand
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

      [[nodiscard]] SortLayer getSortLayer() const override
      {
         return SortLayer::Occurrences;
      }

      void renderBaseTo(contomap::frontend::MapRenderer &renderer) const override
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

   class AssociationRenderCommand : public TypedRenderCommand
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

      [[nodiscard]] SortLayer getSortLayer() const override
      {
         return SortLayer::Associations;
      }

      void renderBaseTo(contomap::frontend::MapRenderer &renderer) const override
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

   class RoleRenderCommand : public TypedRenderCommand
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

      [[nodiscard]] SortLayer getSortLayer() const override
      {
         return SortLayer::Roles;
      }

      void renderBaseTo(contomap::frontend::MapRenderer &renderer) const override
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

   void startNewCommand(std::unique_ptr<TypedRenderCommand> command);
   void addToLastCommand(std::unique_ptr<RenderCommand> command);
   void flushPendingCommand();

   std::list<std::unique_ptr<TypedRenderCommand>> commands;
   std::unique_ptr<TypedRenderCommand> pendingCommand;
};

} // namespace contomap::frontend
