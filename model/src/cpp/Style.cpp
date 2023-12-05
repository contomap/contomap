#include "contomap/model/Style.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Style;

Style::Color const Style::DEFAULT_COLOR { .red = 0x00, .green = 0x00, .blue = 0x00, .alpha = 0x00 };

Style::Color Style::Color::from(Decoder &coder, std::string const &name)
{
   Coder::Scope scope(coder, name);
   Color instance { .red = 0x00, .green = 0x00, .blue = 0x00, .alpha = 0x00 };
   coder.code("r", instance.red);
   coder.code("g", instance.green);
   coder.code("b", instance.blue);
   coder.code("a", instance.alpha);
   return instance;
}

void Style::Color::encode(Encoder &coder, std::string const &name) const
{
   Coder::Scope scope(coder, name);
   coder.code("r", red);
   coder.code("g", green);
   coder.code("b", blue);
   coder.code("a", alpha);
}

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

void Style::encode(Encoder &coder, std::string const &name) const
{
   Coder::Scope scope(coder, name);
   coder.codeArray("colors", colors.begin(), colors.end(), [](Encoder &nested, auto const &kvp) {
      Coder::Scope nestedScope(nested, "");
      nested.code("type", typeToSerial(kvp.first));
      kvp.second.encode(nested, "color");
   });
}

void Style::decode(Decoder &coder, std::string const &name, uint8_t)
{
   Coder::Scope scope(coder, name);
   coder.codeArray("colors", [this](Decoder &nested, size_t) {
      Coder::Scope nestedScope(nested, "");
      uint8_t serialType = 0x00;
      nested.code("type", serialType);
      auto color = Color::from(nested, "color");
      colors.emplace(typeFromSerial(serialType), color);
   });
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

Style::ColorType Style::typeFromSerial(uint8_t value)
{
   static std::map<uint8_t, ColorType> CONSTANTS { { 0x01, ColorType::Line }, { 0x02, ColorType::Fill }, { 0x03, ColorType::Text } };
   return CONSTANTS.at(value);
}

uint8_t Style::typeToSerial(ColorType type)
{
   static std::map<ColorType, uint8_t> CONSTANTS { { ColorType::Line, 0x01 }, { ColorType::Fill, 0x02 }, { ColorType::Text, 0x03 } };
   return CONSTANTS.at(type);
}
