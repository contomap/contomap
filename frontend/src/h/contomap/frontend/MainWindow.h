#pragma once

#include <cstdint>

#include "contomap/frontend/DisplayEnvironment.h"

namespace contomap::frontend
{

class MainWindow
{
public:
   class Size
   {
   public:
      Size(uint32_t width, uint32_t height);

      [[nodiscard]] uint32_t getWidth() const;
      [[nodiscard]] uint32_t getHeight() const;

   private:
      uint32_t width;
      uint32_t height;
   };

   static Size const DEFAULT_SIZE;
   static char const DEFAULT_TITLE[];

   explicit MainWindow(DisplayEnvironment &environment);

   void closeRequested();
   void drawFrame();

private:
   DisplayEnvironment &environment;
};

} // namespace contomap::frontend
