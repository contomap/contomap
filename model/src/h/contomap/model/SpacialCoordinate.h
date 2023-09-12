#pragma once

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
