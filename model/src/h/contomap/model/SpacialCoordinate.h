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

private:
   AbsolutePoint absoluteReference;
};

}
