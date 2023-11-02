#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include <raylib.h>

#include <contomap/frontend/FrameTime.h>

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

      /**
       * Spaceship operator.
       *
       * @param other the other instance to compare to.
       * @return the ordering for this type.
       */
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
       * Updates the internal state of the gearbox on given time passed.
       *
       * @param amount how much time has passed since the previous invocation.
       */
      virtual void timePassed(contomap::frontend::FrameTime amount) = 0;

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

      /**
       * @return the current position as per movement.
       */
      [[nodiscard]] virtual Vector2 getCurrentPosition() const = 0;
      /**
       * Requests to pan to the requested position.
       *
       * @param target the target to center on.
       */
      virtual void panTo(Vector2 target) = 0;
      /**
       * Performs a continuous panning. The regular call to timePassed()
       * will update the current position, until the respective direction is off.
       *
       * @param left pan left.
       * @param up pan up.
       * @param right pan right.
       * @param down pan down.
       */
      virtual void pan(bool left, bool up, bool right, bool down) = 0;
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

      void timePassed(contomap::frontend::FrameTime amount) override;

      void setTargetZoomFactor(ZoomFactor target) override;
      [[nodiscard]] ZoomFactor getTargetZoomFactor() const override;
      [[nodiscard]] ZoomFactor getCurrentZoomFactor() const override;

      [[nodiscard]] Vector2 getCurrentPosition() const override;
      void panTo(Vector2 target) override;
      void pan(bool left, bool up, bool right, bool down) override;

   private:
      Vector2 position;
      ZoomFactor zoomFactor;
      bool panningLeft = false;
      bool panningUp = false;
      bool panningRight = false;
      bool panningDown = false;
   };

   /**
    * Projection is a helper object that keeps a mode active as long as it is within scope.
    */
   class Projection
   {
   public:
      Projection() = delete;
      Projection(Projection const &) = delete;
      /**
       * Move constructor.
       *
       * @param other the other projection to take over from.
       */
      Projection(Projection &&other) noexcept;
      /**
       * Constructor to begin a projection mode.
       *
       * @param data the projection information to use.
       */
      explicit Projection(Camera2D data);
      ~Projection();

      Projection &operator=(Projection const &) = delete;
      /**
       * Move assignment operator.
       *
       * @param other the other projection to take over from.
       * @return this
       */
      Projection &operator=(Projection &&other) noexcept;

      /**
       * Calculate the model coordinate for given pixel location.
       *
       * @param pixel the projected location the camera's lens.
       * @return the nearest coordinate in model space.
       */
      [[nodiscard]] Vector2 unproject(Vector2 pixel) const;

   private:
      void moveFrom(Projection &&other) noexcept;

      Camera2D cameraData;
      bool active = false;
   };

   /**
    * ZoomOperation returns a new target zoom factor based on a current value.
    */
   using ZoomOperation = std::function<ZoomFactor(ZoomFactor)>;

   /**
    * The position that is at the center of a map.
    */
   static Vector2 const HOME_POSITION;
   /**
    * The speed (in pixel/second) to reach with panning over time.
    */
   static float const PANNING_SPEED;

   /**
    * Constructor.
    *
    * @param gearbox the gearbox to use for operations.
    */
   explicit MapCamera(std::shared_ptr<Gearbox> gearbox);

   /**
    * Updates the internal state on given time passed.
    *
    * @param amount how much time has passed since the previous invocation.
    */
   void timePassed(contomap::frontend::FrameTime amount);

   /**
    * Apply the given zoom operation on the current target factor.
    *
    * @param op the operation to apply.
    */
   void zoom(ZoomOperation const &op);

   /**
    * Requests to pan to the specified position.
    *
    * @param target the position to pan to.
    */
   void panTo(Vector2 target);
   /**
    * Perform a continuous panning.
    *
    * @param left pan left.
    * @param up pan up.
    * @param right pan right.
    * @param down pan down.
    */
   void pan(bool left, bool up, bool right, bool down);

   /**
    * @return the current position as per gearbox movement.
    */
   [[nodiscard]] Vector2 getCurrentPosition() const;

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
