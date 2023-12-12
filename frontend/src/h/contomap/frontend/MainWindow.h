#pragma once

#include <cstdint>
#include <memory>

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/editor/SelectionAction.h"
#include "contomap/editor/View.h"
#include "contomap/frontend/Dialog.h"
#include "contomap/frontend/DisplayEnvironment.h"
#include "contomap/frontend/Focus.h"
#include "contomap/frontend/Layout.h"
#include "contomap/frontend/MapCamera.h"
#include "contomap/frontend/MapRenderer.h"
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
   static Size const DEFAULT_SIZE;
   static char const DEFAULT_TITLE[];
   static std::array<char, 5> const PNG_MAP_TYPE;

   [[nodiscard]] static contomap::frontend::MapCamera::ZoomOperation doubledRelative(bool nearer);
   [[nodiscard]] static std::vector<std::pair<int, contomap::frontend::MapCamera::ZoomFactor>> generateZoomLevels();

   void processInput(Vector2 focusCoordinate, Vector2 focusDelta);
   void updateState();
   void cycleSelectedOccurrence(bool forward);
   void jumpToFirstOccurrenceOf(contomap::model::Identifier topicId);
   void panCameraToSelectedOccurrence();

   void drawBackground();
   void drawMap(Vector2 focusCoordinate);
   void drawUserInterface(contomap::frontend::RenderContext const &context);

   void renderMap(contomap::frontend::MapRenderer &renderer, contomap::editor::Selection const &selection, Focus const &focus);

   void requestNewFile();
   void requestLoad();
   void requestSave();

   void closeDialog();
   void openHelpDialog();
   void openNewTopicDialog();
   void openNewOccurrenceDialog();
   void openNewLocateTopicAndActDialog();
   void openSetTopicNameDefaultDialog();
   void openSetTopicNameInScopeDialog();
   void openEditStyleDialog();

   void load(std::string const &filePath);
   void save();
   void mapRestored(std::string const &filePath);

   [[nodiscard]] static contomap::model::Style selectedStyle(contomap::model::Style style);
   [[nodiscard]] static contomap::model::Style highlightedStyle(contomap::model::Style style);
   [[nodiscard]] static contomap::model::Style::Color brightenColor(contomap::model::Style::Color base, float factor);

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

   contomap::frontend::Focus currentFocus;
   std::string currentFilePath;

   Vector2 lastMousePos;
};

} // namespace contomap::frontend
