#pragma once

#include <cstdint>

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/frontend/DisplayEnvironment.h"
#include "contomap/frontend/ViewModelState.h"

namespace contomap::frontend
{

class MainWindow
{
public:
   class LengthInPixel
   {
   public:
      using ValueType = uint32_t;

      static LengthInPixel of(ValueType value);

      template <typename TargetType = ValueType> [[nodiscard]] TargetType raw() const
      {
         return static_cast<TargetType>(value);
      }

   private:
      explicit LengthInPixel(ValueType value);

      ValueType value;
   };

   class Size
   {
   public:
      static Size ofPixel(LengthInPixel::ValueType width, LengthInPixel::ValueType height);

      [[nodiscard]] LengthInPixel getWidth() const;
      [[nodiscard]] LengthInPixel getHeight() const;

   private:
      Size(MainWindow::LengthInPixel width, MainWindow::LengthInPixel height);

      LengthInPixel width;
      LengthInPixel height;
   };

   static Size const DEFAULT_SIZE;
   static char const DEFAULT_TITLE[];

   MainWindow(DisplayEnvironment &environment, contomap::editor::InputRequestHandler &inputRequestHandler);

   contomap::editor::ViewModel &viewModel();

   void init();
   void closeRequested();
   void drawFrame();
   void close();

private:
   void drawBackground();
   void drawMap();
   void drawUserInterface();

   contomap::frontend::ViewModelState viewModelState;

   contomap::frontend::DisplayEnvironment &environment;
   contomap::editor::InputRequestHandler &inputRequestHandler;
};

} // namespace contomap::frontend
