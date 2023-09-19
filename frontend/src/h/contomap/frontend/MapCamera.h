#pragma once

#include <raylib.h>

namespace contomap::frontend
{

/**
 * MapCamera represents the current projection transformation for display.
 */
class MapCamera
{
public:
   /**
    * Projection is a helper object that keeps a mode active as long as it is within scope.
    */
   class Projection
   {
   public:
      Projection() = delete;
      Projection(Projection const &) = delete;
      Projection(Projection &&other) noexcept;
      /**
       * Constructor to begin a projection mode.
       *
       * @param data the projection information to use.
       */
      explicit Projection(Camera2D data);
      ~Projection();

      Projection &operator=(Projection const &) = delete;
      Projection &operator=(Projection &&other) noexcept;

   private:
      void moveFrom(Projection &&other) noexcept;

      bool active = false;
   };

   /**
    * Default constructor.
    */
   MapCamera();

   /**
    * Enters the projection mode; Drawing operations will be based on the projection transformation.
    *
    * @param viewportSize describes the size of the available space.
    * @return a projection object. When the object goes out of scope, the mode is reset.
    */
   Projection beginProjection(Vector2 viewportSize);

private:
   Camera2D data {};
};

}
