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
};

}
