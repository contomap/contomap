#pragma once

#include <cstdint>
#include <functional>
#include <memory>

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
    * ZoomFactor describes a grade of magnification or minification.
    */
   class ZoomFactor
   {
   public:
      /** The limit of the farthest zoom - the most minification possible. */
      static ZoomFactor const FAR_LIMIT;
      /** The unit value - the reference zoom level. */
      static ZoomFactor const UNIT;
      /** The limit of the nearest zoom - the most magnification possible. */
      static ZoomFactor const NEAR_LIMIT;

      /**
       * Deleted default constructor.
       */
      ZoomFactor() = delete;

      /**
       * Create an instance based on the given raw floating point value.
       * The function performs clamping of given value based on the near and far limits.
       *
       * @param value the raw value.
       * @return the resulting zoom factor.
       */
      [[nodiscard]] static ZoomFactor from(float value);

      /**
       * @return the raw factor of this instance.
       */
      [[nodiscard]] float raw() const;

      std::strong_ordering operator<=>(ZoomFactor const &other) const noexcept = default;

   private:
      using StorageType = uint32_t;

      explicit ZoomFactor(StorageType value);

      StorageType value;
   };

   /**
    * Gearbox provides the abstraction of changes within the camera.
    * The gearbox is requested to reach a certain target, and depending on the implementation,
    * this can be an immediate application of the requested value, or a gradual movement
    * towards the intended value.
    */
   class Gearbox
   {
   public:
      virtual ~Gearbox() = default;

      /**
       * Sets the target zoom factor for the camera.
       *
       * @param target the zoom factor to reach.
       */
      virtual void setTargetZoomFactor(ZoomFactor target) = 0;
      /**
       * @return the target zoom factor as per previous request.
       */
      [[nodiscard]] virtual ZoomFactor getTargetZoomFactor() const = 0;
      /**
       * @return the current zoom factor as per movement.
       */
      [[nodiscard]] virtual ZoomFactor getCurrentZoomFactor() const = 0;
   };

   /**
    * This Gearbox implementation applies all changes immediately.
    */
   class ImmediateGearbox : public Gearbox
   {
   public:
      /**
       * Default constructor.
       */
      ImmediateGearbox();

      void setTargetZoomFactor(ZoomFactor target) override;
      [[nodiscard]] ZoomFactor getTargetZoomFactor() const override;
      [[nodiscard]] ZoomFactor getCurrentZoomFactor() const override;

   private:
      ZoomFactor zoomFactor;
   };

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
    * ZoomOperation returns a new target zoom factor based on a current value.
    */
   using ZoomOperation = std::function<ZoomFactor(ZoomFactor)>;

   /**
    * Constructor.
    *
    * @param gearbox the gearbox to use for operations.
    */
   explicit MapCamera(std::shared_ptr<Gearbox> gearbox);

   /**
    * Apply the given zoom operation on the current target factor.
    *
    * @param op the operation to apply.
    */
   void zoom(ZoomOperation op);

   /**
    * Enters the projection mode; Drawing operations will be based on the projection transformation.
    *
    * @param viewportSize describes the size of the available space.
    * @return a projection object. When the object goes out of scope, the mode is reset.
    */
   Projection beginProjection(Vector2 viewportSize);

private:
   std::shared_ptr<Gearbox> gearbox;
   Camera2D data {};
};

}
