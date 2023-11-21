#pragma once

#include <cstdint>
#include <map>
#include <vector>

namespace contomap::model
{

/**
 * A Style contains information on how to present things.
 */
class Style
{
public:
   /**
    * ColorType identifies a color property.
    */
   enum class ColorType
   {
      /**
       * Text is meant for the primary color of displayed characters.
       */
      Text,
      /**
       * Line is meant for strokes that outline (or represent) a shape.
       */
      Line,
      /**
       * Fill is meant for areas of shapes.
       */
      Fill,
   };

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

   /** The default value returned if not specified. */
   static Color const DEFAULT_COLOR;

   /**
    * Creates an average of all provided styles.
    * @param styles the list of styles from which to create an average.
    * @return a resulting style.
    */
   [[nodiscard]] static Style averageOf(std::vector<Style> const &styles);

   /**
    * Returns a new style with a copy from this style and any optionals not yet filled out are defaulting to given style.
    *
    * @param style the style from which to take values.
    * @return a new style instance.
    */
   [[nodiscard]] Style withDefaultsFrom(Style const &style) const;

   /**
    * Returns a new style with the copy from this style and the identified color overridden.
    *
    * @param type the type of color to override.
    * @param value the value to override.
    * @return a new style instance.
    */
   [[nodiscard]] Style with(ColorType type, Color value) const;

   /**
    * Returns a new style with the copy from this style and the identified color cleared.
    *
    * @param type the type of color to clear.
    * @return a new style instance.
    */
   [[nodiscard]] Style without(ColorType type) const;

   /**
    * Returns whether a particular color has been set in this style.
    *
    * @param type the type of color to test.
    * @return true if the color is defined, false otherwise.
    */
   [[nodiscard]] bool has(ColorType type) const;

   /**
    * @param type the type of color to retrieve.
    * @return the text color if complete, transparent black otherwise.
    */
   [[nodiscard]] Color get(ColorType type, Color defaultValue = DEFAULT_COLOR) const;

private:
   std::map<ColorType, Color> colors;
};

}
