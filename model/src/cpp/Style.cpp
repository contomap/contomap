#include "contomap/model/Style.h"

using contomap::model::Identifier;
using contomap::model::Style;

Style::Color const Style::Set::DEFAULT_TEXT { .red = 0x00, .green = 0x00, .blue = 0x00, .alpha = 0xFF };
Style::Color const Style::Set::DEFAULT_FILL { .red = 0xFF, .green = 0xFF, .blue = 0xFF, .alpha = 0xFF };
Style::Color const Style::Set::DEFAULT_LINE { .red = 0x00, .green = 0x00, .blue = 0x00, .alpha = 0xFF };

Style::Set Style::Set::withTextColor(Color value) const
{
   Set copy(*this);
   copy.textColor = value;
   return copy;
}

Style::Set Style::Set::withFillColor(Color value) const
{
   Set copy(*this);
   copy.fillColor = value;
   return copy;
}

Style::Set Style::Set::withLineColor(Color value) const
{
   Set copy(*this);
   copy.lineColor = value;
   return copy;
}

Style::Set Style::Set::withDefaultsFrom(Style const &style) const
{
   return withDefaultsFrom(style.toSet());
}

Style::Set Style::Set::withDefaultsFrom(Set const &other) const
{
   Set copy(*this);
   if (!copy.textColor.has_value())
   {
      copy.textColor = other.textColor;
   }
   if (!copy.fillColor.has_value())
   {
      copy.fillColor = other.fillColor;
   }
   if (!copy.lineColor.has_value())
   {
      copy.lineColor = other.lineColor;
   }
   return copy;
}

Style::Color Style::Set::getTextColor() const
{
   return textColor.value_or(DEFAULT_TEXT);
}

Style::Color Style::Set::getFillColor() const
{
   return fillColor.value_or(DEFAULT_FILL);
}

Style::Color Style::Set::getLineColor() const
{
   return lineColor.value_or(DEFAULT_LINE);
}

Style::Style(Identifier id)
   : id(id)
{
}

Style::Set const &Style::toSet() const
{
   return set;
}
