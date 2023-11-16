#include "contomap/model/Style.h"

using contomap::model::Style;

Style::Color const Style::DEFAULT_COLOR { .red = 0x00, .green = 0x00, .blue = 0x00, .alpha = 0x00 };

Style Style::withDefaultsFrom(Style const &other) const
{
   Style copy(*this);
   for (auto const &[type, value] : other.colors)
   {
      if (!copy.colors.contains(type))
      {
         copy.colors[type] = value;
      }
   }
   return copy;
}

Style Style::with(ColorType type, Color value) const
{
   Style copy(*this);
   copy.colors[type] = value;
   return copy;
}

Style Style::without(ColorType type) const
{
   Style copy(*this);
   copy.colors.erase(type);
   return copy;
}

bool Style::has(ColorType type) const
{
   return colors.contains(type);
}

Style::Color Style::get(ColorType type, Color defaultColor) const
{
   auto it = colors.find(type);
   return (it != colors.end()) ? it->second : defaultColor;
}
