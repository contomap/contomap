#pragma once

#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"

namespace contomap::model
{

/**
 * SpacialCoordinate describes a location.
 */
class SpacialCoordinate
{
public:
   /** Type for coordinate values. */
   using CoordinateType = float;

   class Offset
   {
   public:
      /**
       * Factory method creating a new instance with the given values.
       *
       * @param x the X delta.
       * @param y the Y delta.
       * @return the resulting instance.
       */
      [[nodiscard]] static Offset of(CoordinateType x, CoordinateType y)
      {
         return { x, y };
      }

      /**
       * Calculate the sum of two offset values.
       *
       * @param other the other offset to add.
       * @return the resulting value.
       */
      [[nodiscard]] Offset plus(Offset other) const
      {
         return { x + other.x, y + other.y };
      }

      /**
       * @return the X delta.
       */
      [[nodiscard]] CoordinateType X() const
      {
         return x;
      }
      /**
       * @return the Y delta.
       */
      [[nodiscard]] CoordinateType Y() const
      {
         return y;
      }

   private:
      Offset(CoordinateType x, CoordinateType y)
         : x(x)
         , y(y)
      {
      }

      CoordinateType x;
      CoordinateType y;
   };

   /**
    * AbsolutePoint is a location at an absolute position based on an origin at x=0,y=0.
    */
   class AbsolutePoint
   {
   public:
      /**
       * Default constructor, initializing at origin.
       */
      AbsolutePoint();

      /**
       * Factory method creating a new instance with the given values.
       *
       * @param x the X coordinate.
       * @param y the Y coordinate.
       * @return the resulting instance.
       */
      [[nodiscard]] static AbsolutePoint at(CoordinateType x, CoordinateType y);

      /**
       * Deserializes the coordinates.
       *
       * @param coder the decoder to use.
       * @param name the name to use for the scope.
       * @return the decoded instance.
       */
      [[nodiscard]] static AbsolutePoint from(contomap::infrastructure::serial::Decoder &coder, std::string const &name);

      /**
       * Calculates a new absolute point by adding the given offset to the current values.
       *
       * @param offset the offset to apply.
       * @return the resulting value.
       */
      [[nodiscard]] AbsolutePoint plus(Offset offset);

      /**
       * Serializes the coordinates.
       *
       * @param coder the encoder to use.
       * @param name the name to use for the scope.
       */
      void encode(contomap::infrastructure::serial::Encoder &coder, std::string const &name) const;

      /**
       * @return the X coordinate.
       */
      [[nodiscard]] CoordinateType X() const
      {
         return x;
      }
      /**
       * @return the Y coordinate.
       */
      [[nodiscard]] CoordinateType Y() const
      {
         return y;
      }

   private:
      AbsolutePoint(CoordinateType x, CoordinateType y);

      CoordinateType x;
      CoordinateType y;
   };

   /**
    * Factory method creating a new instance with the given values.
    *
    * @param x the X coordinate.
    * @param y the Y coordinate.
    * @return the resulting instance.
    */
   [[nodiscard]] static SpacialCoordinate absoluteAt(CoordinateType x, CoordinateType y);

   /**
    * Serializes the coordinates.
    *
    * @param coder the encoder to use.
    * @param name the name to use for the scope.
    */
   void encode(contomap::infrastructure::serial::Encoder &coder, std::string const &name) const;

   /**
    * Deserializes the coordinates.
    *
    * @param coder the decoder to use.
    * @param name the name to use for the scope.
    * @param version the version to consider.
    */
   void decode(contomap::infrastructure::serial::Decoder &coder, std::string const &name, uint8_t version);

   /**
    * @param point the new absolute reference point.
    */
   void setAbsoluteReference(AbsolutePoint point);
   /**
    * @return the current absolute reference point.
    */
   [[nodiscard]] AbsolutePoint getAbsoluteReference() const;

   /**
    * Move the spacial coordinate by given offset.
    *
    * @param offset the offset to apply.
    */
   void moveBy(Offset offset);

private:
   AbsolutePoint absoluteReference;
};

}
