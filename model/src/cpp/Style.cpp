#include "contomap/model/Style.h"

using contomap::model::Style;

Style::Color const Style::DEFAULT_COLOR { .red = 0x00, .green = 0x00, .blue = 0x00, .alpha = 0x00 };

Style Style::averageOf(std::vector<Style> const &styles)
{
   if (styles.empty())
   {
      return {};
   }
   if (styles.size() == 1)
   {
      return styles[0];
   }

   struct AccumulatedColor
   {
      uint32_t red = 0;
      uint32_t green = 0;
      uint32_t blue = 0;
      uint32_t alpha = 0;
      uint32_t count = 0;
   };
   std::map<ColorType, AccumulatedColor> accumulated;
   for (auto const &style : styles)
   {
      for (auto const &[type, value] : style.colors)
      {
         auto &acc = accumulated[type];
         acc.red += static_cast<uint32_t>(value.red);
         acc.green += static_cast<uint32_t>(value.green);
         acc.blue += static_cast<uint32_t>(value.blue);
         acc.alpha += static_cast<uint32_t>(value.alpha);
         acc.count++;
      }
   }
   Style result;
   for (auto &[type, acc] : accumulated)
   {
      result.colors[type] = Color {
         .red = static_cast<uint8_t>(acc.red / acc.count),
         .green = static_cast<uint8_t>(acc.green / acc.count),
         .blue = static_cast<uint8_t>(acc.blue / acc.count),
         .alpha = static_cast<uint8_t>(acc.alpha / acc.count),
      };
   }
   return result;
}

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
