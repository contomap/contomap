#include <cstring>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include <raylib.h>
#include <raymath.h>
#pragma GCC diagnostic pop

#include "contomap/frontend/MapCamera.h"

using contomap::frontend::FrameTime;
using contomap::frontend::MapCamera;

MapCamera::ZoomFactor const MapCamera::ZoomFactor::FAR_LIMIT(1000);
MapCamera::ZoomFactor const MapCamera::ZoomFactor::UNIT(1000000);
MapCamera::ZoomFactor const MapCamera::ZoomFactor::NEAR_LIMIT(1000000000);

MapCamera::ZoomFactor MapCamera::ZoomFactor::from(float value)
{
   float clampedValue = Clamp(value, FAR_LIMIT.raw(), NEAR_LIMIT.raw());
   auto roundedValue = static_cast<StorageType>(std::round(clampedValue * static_cast<float>(UNIT.value)));
   return ZoomFactor(roundedValue);
}

float MapCamera::ZoomFactor::raw() const
{
   return static_cast<float>(value) / static_cast<float>(UNIT.value);
}

MapCamera::ZoomFactor::ZoomFactor(StorageType value)
   : value(value)
{
}

MapCamera::ImmediateGearbox::ImmediateGearbox()
   : position(MapCamera::HOME_POSITION)
   , zoomFactor(ZoomFactor::UNIT)
{
}

void MapCamera::ImmediateGearbox::timePassed(FrameTime amount)
{
   Vector2 v {
      .x = (panningLeft ? -1.0f : 0.0f) + (panningRight ? 1.0f : 0.0f),
      .y = (panningUp ? -1.0f : 0.0f) + (panningDown ? 1.0f : 0.0f),
   };
   auto normalized = Vector2Normalize(v);
   position = Vector2Add(position, Vector2Scale(normalized, (MapCamera::PANNING_SPEED * amount.rawSeconds()) / zoomFactor.raw()));
}

void MapCamera::ImmediateGearbox::setTargetZoomFactor(ZoomFactor target)
{
   zoomFactor = target;
}

MapCamera::ZoomFactor MapCamera::ImmediateGearbox::getTargetZoomFactor() const
{
   return zoomFactor;
}

MapCamera::ZoomFactor MapCamera::ImmediateGearbox::getCurrentZoomFactor() const
{
   return zoomFactor;
}

Vector2 MapCamera::ImmediateGearbox::getTargetPosition() const
{
   return position;
}

Vector2 MapCamera::ImmediateGearbox::getCurrentPosition() const
{
   return position;
}

void MapCamera::ImmediateGearbox::panTo(Vector2 target)
{
   position = target;
}

void MapCamera::ImmediateGearbox::pan(bool left, bool up, bool right, bool down)
{
   panningLeft = left;
   panningUp = up;
   panningRight = right;
   panningDown = down;
}

MapCamera::SmoothGearbox::SmoothGearbox()
   : currentPosition(MapCamera::HOME_POSITION)
   , requestedZoomFactor(ZoomFactor::UNIT)
   , targetZoomFactor(ZoomFactor::UNIT)
   , currentZoomFactor(ZoomFactor::UNIT)
   , currentPanningSpeed { .x = 0.0f, .y = 0.0f }
{
}

void MapCamera::SmoothGearbox::timePassed(FrameTime amount)
{
   targetZoomFactor = requestedZoomFactor;
   float zoomOffset = targetZoomFactor.raw() - currentZoomFactor.raw();
   if ((std::abs(zoomOffset) > 0.001f) && (amount.rawSeconds() < ZOOM_TARGET_TIME))
   {
      currentZoomFactor = ZoomFactor::from(currentZoomFactor.raw() + ((zoomOffset / ZOOM_TARGET_TIME) * amount.rawSeconds()));
   }
   else
   {
      currentZoomFactor = targetZoomFactor;
   }

   if (requestedPosition.has_value())
   {
      targetPosition = requestedPosition;
      requestedPosition.reset();
   }
   if (targetPosition.has_value())
   {
      Vector2 positionOffset = Vector2Subtract(targetPosition.value(), currentPosition);
      if ((Vector2Length(positionOffset) > 0.001f) && (amount.rawSeconds() < PANNING_TARGET_TIME))
      {
         currentPosition = Vector2Add(currentPosition, Vector2Scale(positionOffset, amount.rawSeconds() / PANNING_TARGET_TIME));
      }
      else
      {
         currentPosition = targetPosition.value();
         targetPosition.reset();
      }
   }

   Vector2 v {
      .x = (panningLeft ? -1.0f : 0.0f) + (panningRight ? 1.0f : 0.0f),
      .y = (panningUp ? -1.0f : 0.0f) + (panningDown ? 1.0f : 0.0f),
   };
   auto targetPanningSpeed = Vector2Normalize(v);
   Vector2 panningSpeedOffset = Vector2Subtract(targetPanningSpeed, currentPanningSpeed);
   if ((Vector2Length(panningSpeedOffset) > 0.001f) && (amount.rawSeconds() < PANNING_TARGET_TIME))
   {
      currentPanningSpeed = Vector2Add(currentPanningSpeed, Vector2Scale(panningSpeedOffset, amount.rawSeconds() / PANNING_TARGET_TIME));
   }
   else
   {
      currentPanningSpeed = targetPanningSpeed;
   }
   currentPosition = Vector2Add(currentPosition, Vector2Scale(currentPanningSpeed, (MapCamera::PANNING_SPEED * amount.rawSeconds()) / currentZoomFactor.raw()));
}

void MapCamera::SmoothGearbox::setTargetZoomFactor(ZoomFactor target)
{
   requestedZoomFactor = target;
}

MapCamera::ZoomFactor MapCamera::SmoothGearbox::getTargetZoomFactor() const
{
   return targetZoomFactor;
}

MapCamera::ZoomFactor MapCamera::SmoothGearbox::getCurrentZoomFactor() const
{
   return currentZoomFactor;
}

Vector2 MapCamera::SmoothGearbox::getTargetPosition() const
{
   return targetPosition.has_value() ? targetPosition.value() : currentPosition;
}

Vector2 MapCamera::SmoothGearbox::getCurrentPosition() const
{
   return currentPosition;
}

void MapCamera::SmoothGearbox::panTo(Vector2 target)
{
   requestedPosition = target;
}

void MapCamera::SmoothGearbox::pan(bool left, bool up, bool right, bool down)
{
   panningLeft = left;
   panningUp = up;
   panningRight = right;
   panningDown = down;
   if (left || up || right || down)
   {
      requestedPosition.reset();
      targetPosition.reset();
   }
}

contomap::frontend::MapCamera::Projection::Projection(MapCamera::Projection &&other) noexcept
{
   moveFrom(std::move(other));
}

MapCamera::Projection::Projection(Camera2D data)
   : cameraData(data)
{
   BeginMode2D(cameraData);
   active = true;
}

MapCamera::Projection::~Projection()
{
   if (active)
   {
      EndMode2D();
   }
}

MapCamera::Projection &contomap::frontend::MapCamera::Projection::operator=(MapCamera::Projection &&other) noexcept
{
   moveFrom(std::move(other));
   return *this;
}

Vector2 MapCamera::Projection::unproject(Vector2 pixel) const
{
   return GetScreenToWorld2D(pixel, cameraData);
}

void MapCamera::Projection::moveFrom(Projection &&other) noexcept
{
   cameraData = other.cameraData;
   if (other.active)
   {
      active = true;
      other.active = false;
   }
}

Vector2 const MapCamera::HOME_POSITION { .x = 0.0f, .y = 0.0f };
float const MapCamera::PANNING_SPEED = 800.0f;

MapCamera::MapCamera(std::shared_ptr<Gearbox> gearbox)
   : gearbox(std::move(gearbox))
{
   memset(&data, 0x00, sizeof(data));
}

void MapCamera::timePassed(FrameTime amount)
{
   gearbox->timePassed(amount);
}

void MapCamera::zoom(MapCamera::ZoomOperation const &op)
{
   gearbox->setTargetZoomFactor(op(gearbox->getTargetZoomFactor()));
}

void MapCamera::panTo(Vector2 target)
{
   gearbox->panTo(target);
}

void MapCamera::pan(bool left, bool up, bool right, bool down)
{
   gearbox->pan(left, up, right, down);
}

MapCamera::Projection MapCamera::beginProjection(Vector2 viewportSize)
{
   Vector2 const &centerOfViewport = Vector2Scale(viewportSize, 0.5f);
   data.offset = centerOfViewport;
   data.target = gearbox->getCurrentPosition();
   data.zoom = gearbox->getCurrentZoomFactor().raw();
   return Projection { data };
}

Vector2 MapCamera::getTargetPosition() const
{
   return gearbox->getTargetPosition();
}

Vector2 MapCamera::getCurrentPosition() const
{
   return gearbox->getCurrentPosition();
}