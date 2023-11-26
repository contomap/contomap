#pragma once

#include <raygui/raygui.h>

#include "contomap/model/Style.h"

namespace contomap::frontend
{

/**
 * Colors provides helpers for color operation.
 */
class Colors
{
public:
   /**
    * Converts a color from the model to the frontend type.
    *
    * @param value the value to convert.
    * @return the equivalent type.
    */
   [[nodiscard]] static Color toUiColor(contomap::model::Style::Color value)
   {
      return Color { .r = value.red, .g = value.green, .b = value.blue, .a = value.alpha };
   }

   /**
    * Converts a color from the frontend type to the model.
    *
    * @param value the value to convert.
    * @return the equivalent type.
    */
   [[nodiscard]] static contomap::model::Style::Color fromUiColor(Color value)
   {
      return contomap::model::Style::Color { .red = value.r, .green = value.g, .blue = value.b, .alpha = value.a };
   }
};

}
