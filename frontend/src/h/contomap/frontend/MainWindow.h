#pragma once

#include <cstdint>
#include <memory>

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/editor/SelectionAction.h"
#include "contomap/editor/View.h"
#include "contomap/frontend/Dialog.h"
#include "contomap/frontend/DisplayEnvironment.h"
#include "contomap/frontend/Layout.h"
#include "contomap/frontend/MapCamera.h"
#include "contomap/frontend/RenderContext.h"

namespace contomap::frontend
{

/**
 * MainWindow represents the primary window, embedded in the DisplayEnvironment.
 */
class MainWindow
{
public:
   /**
    * LengthInPixel is a distance that uses pixels as unit.
    */
   class LengthInPixel
   {
   public:
      /** ValueType is the container for the value. */
      using ValueType = uint32_t;

      /**
       * Factory function for LengthInPixel.
       *
       * @param value the raw value to use for the resulting instance.
       * @return an instance representing the given value.
       */
      static LengthInPixel of(ValueType value);

      /**
       * Perform a cast towards a specified target type.
       *
       * @tparam TargetType the type, such as "int" to cast towards.
       * @return the raw value, cast to the requested type.
       */
      template <typename TargetType = ValueType> [[nodiscard]] TargetType raw() const
      {
         return static_cast<TargetType>(value);
      }

   private:
      explicit LengthInPixel(ValueType value);

      ValueType value;
   };

   /**
    * Size represents the horizontal and vertical extend of a window.
    */
   class Size
   {
   public:
      /**
       * Factory function to create an instance.
       *
       * @param width the width of the size, in pixel.
       * @param height the height of the size, in pixel.
       * @return a new instance.
       */
      static Size ofPixel(LengthInPixel::ValueType width, LengthInPixel::ValueType height);

      /**
       * @return the width of the size.
       */
      [[nodiscard]] LengthInPixel getWidth() const;
      /**
       * @return the height of the size.
       */
      [[nodiscard]] LengthInPixel getHeight() const;

   private:
      Size(MainWindow::LengthInPixel width, MainWindow::LengthInPixel height);

      LengthInPixel width;
      LengthInPixel height;
   };

   /**
    * Constructor.
    *
    * @param environment the environment that the window runs in.
    * @param view the view on the current editor state for display.
    * @param inputRequestHandler the handler for notifying input requests.
    */
   MainWindow(DisplayEnvironment &environment, contomap::editor::View &view, contomap::editor::InputRequestHandler &inputRequestHandler);

   /**
    * Initializes and opens the window in the display environment.
    */
   void init();

   /**
    * Called if, through external request, the application should shut down.
    */
   void closeRequested();

   /**
    * This method is periodically called to process the next frame: handle state changes and draw the next frame.
    */
   void nextFrame();

   /**
    * Close the window in the display environment.
    */
   void close();

private:
   class FocusItem
   {
   public:
      virtual ~FocusItem() = default;

      [[nodiscard]] virtual bool isAssociation([[maybe_unused]] contomap::model::Identifier otherId) const
      {
         return false;
      }
      [[nodiscard]] virtual bool isRole([[maybe_unused]] contomap::model::Identifier otherId) const
      {
         return false;
      }
      [[nodiscard]] virtual bool isOccurrence([[maybe_unused]] contomap::model::Identifier otherId) const
      {
         return false;
      }
      virtual void modifySelection(contomap::editor::InputRequestHandler &handler, contomap::editor::SelectionAction action) const = 0;
   };

   class AssociationFocusItem : public FocusItem
   {
   public:
      explicit AssociationFocusItem(contomap::model::Identifier id)
         : id(id)
      {
      }

      [[nodiscard]] bool isAssociation(contomap::model::Identifier otherId) const override
      {
         return id == otherId;
      }

      void modifySelection(contomap::editor::InputRequestHandler &handler, contomap::editor::SelectionAction action) const override
      {
         handler.modifySelection(contomap::editor::SelectedType::Association, id, action);
      }

   private:
      contomap::model::Identifier id;
   };

   class RoleFocusItem : public FocusItem
   {
   public:
      explicit RoleFocusItem(contomap::model::Identifier id)
         : id(id)
      {
      }

      [[nodiscard]] bool isRole(contomap::model::Identifier otherId) const override
      {
         return id == otherId;
      }

      void modifySelection(contomap::editor::InputRequestHandler &handler, contomap::editor::SelectionAction action) const override
      {
         handler.modifySelection(contomap::editor::SelectedType::Role, id, action);
      }

   private:
      contomap::model::Identifier id;
   };

   class OccurrenceFocusItem : public FocusItem
   {
   public:
      explicit OccurrenceFocusItem(contomap::model::Identifier id)
         : id(id)
      {
      }

      [[nodiscard]] bool isOccurrence(contomap::model::Identifier otherId) const override
      {
         return id == otherId;
      }

      void modifySelection(contomap::editor::InputRequestHandler &handler, contomap::editor::SelectionAction action) const override
      {
         handler.modifySelection(contomap::editor::SelectedType::Occurrence, id, action);
      }

   private:
      contomap::model::Identifier id;
   };

   class Focus
   {
   public:
      Focus();

      void registerItem(std::shared_ptr<FocusItem> newItem, float newDistance);

      [[nodiscard]] bool isAssociation(contomap::model::Identifier otherId) const
      {
         return (item != nullptr) && item->isAssociation(otherId);
      }
      [[nodiscard]] bool isRole(contomap::model::Identifier otherId) const
      {
         return (item != nullptr) && item->isRole(otherId);
      }
      [[nodiscard]] virtual bool isOccurrence(contomap::model::Identifier otherId) const
      {
         return (item != nullptr) && item->isOccurrence(otherId);
      }

      void modifySelection(contomap::editor::InputRequestHandler &handler, contomap::editor::SelectionAction action) const;

   private:
      std::shared_ptr<FocusItem> item;
      float distance;
   };

   static Size const DEFAULT_SIZE;
   static char const DEFAULT_TITLE[];
   static std::vector<std::pair<int, contomap::frontend::MapCamera::ZoomFactor>> const ZOOM_LEVELS;

   [[nodiscard]] static std::vector<std::pair<int, contomap::frontend::MapCamera::ZoomFactor>> generateZoomLevels();
   [[nodiscard]] static contomap::frontend::MapCamera::ZoomOperation doubledRelative(bool nearer);

   void processInput();
   void updateState();
   void cycleSelectedOccurrence(bool forward);

   void drawBackground();
   void drawMap(RenderContext const &context);
   void drawUserInterface(contomap::frontend::RenderContext const &context);

   void closeDialog();
   void openHelpDialog();
   void openNewTopicDialog();
   void openNewOccurrenceDialog();
   void openNewLocateTopicAndActDialog();

   [[nodiscard]] contomap::model::SpacialCoordinate spacialCameraLocation();
   [[nodiscard]] std::string bestTitleFor(contomap::model::Topic const &topic);

   contomap::frontend::Layout layout;

   contomap::frontend::MapCamera mapCamera;

   contomap::frontend::DisplayEnvironment &environment;
   contomap::editor::View &view;
   contomap::editor::InputRequestHandler &inputRequestHandler;

   contomap::model::Identifiers lastViewScope;
   size_t viewScopeListStartIndex = 0;

   std::unique_ptr<contomap::frontend::Dialog> currentDialog;
   std::unique_ptr<contomap::frontend::Dialog> pendingDialog;

   Focus currentFocus;
};

} // namespace contomap::frontend
