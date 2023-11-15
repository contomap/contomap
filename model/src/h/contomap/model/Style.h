#pragma once

#include <cstdint>

#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/OptionalIdentifier.h"
#include "contomap/model/TopicNameValue.h"

namespace contomap::model
{

/**
 * A Style contains information on how to present things.
 */
class Style
{
public:
   /**
    * Color provides the intensities of the red, green, and blue color channels, as well as an alpha value for blending.
    */
   struct Color
   {
   public:
      /** Red color channel intensity. */
      uint8_t red;
      /** Green color channel intensity. */
      uint8_t green;
      /** Blue color channel intensity. */
      uint8_t blue;
      /** Alpha blending intensity. */
      uint8_t alpha;
   };

   /**
    * Set represents the sole properties for rendering of a style.
    */
   class Set
   {
   public:
      /**
       * Default constructor.
       */
      Set() = default;

      /**
       * Returns a new set with the copy from this set and the text color overridden.
       *
       * @param value the value to override.
       * @return a new set instance.
       */
      [[nodiscard]] Set withTextColor(Color value) const;

      /**
       * Returns a new set with the copy from this set and the fill color overridden.
       *
       * @param value the value to override.
       * @return a new set instance.
       */
      [[nodiscard]] Set withFillColor(Color value) const;

      /**
       * Returns a new set with the copy from this set and the line color overridden.
       *
       * @param value the value to override.
       * @return a new set instance.
       */
      [[nodiscard]] Set withLineColor(Color value) const;

      /**
       * Returns a new set with a copy from this set and any optionals not yet filled out are defaulting to given style.
       *
       * @param style the style from which to take values.
       * @return a new set instance.
       */
      [[nodiscard]] Set withDefaultsFrom(Style const &style) const;

      /**
       * Returns a new set with a copy from this set and any optionals not yet filled out are defaulting to given set.
       *
       * @param other the other set from which to take values.
       * @return a new set instance.
       */
      [[nodiscard]] Set withDefaultsFrom(Set const &other) const;

      /**
       * @return the text color if complete, black otherwise.
       */
      [[nodiscard]] Color getTextColor() const;

      /**
       * @return the fill color if complete, white otherwise.
       */
      [[nodiscard]] Color getFillColor() const;

      /**
       * @return the line color if complete, black otherwise.
       */
      [[nodiscard]] Color getLineColor() const;

   private:
      static Color const DEFAULT_TEXT;
      static Color const DEFAULT_FILL;
      static Color const DEFAULT_LINE;

      std::optional<Color> textColor;
      std::optional<Color> fillColor;
      std::optional<Color> lineColor;
   };

   /**
    * Constructor.
    *
    * @param id the primary identifier of this style.
    */
   explicit Style(contomap::model::Identifier id);

   /**
    * @return the set of parameters for this style.
    */
   [[nodiscard]] Set const &toSet() const;

private:
   contomap::model::Identifier id;
   contomap::model::OptionalIdentifier localTo;

   Set set;
};

}
