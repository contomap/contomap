#include <cstring>

#include <raylib.h>
#include <raymath.h>

#include "contomap/frontend/MapCamera.h"

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

Vector2 MapCamera::ImmediateGearbox::getCurrentPosition() const
{
   return position;
}

void MapCamera::ImmediateGearbox::panTo(Vector2 target)
{
   position = target;
}

void MapCamera::ImmediateGearbox::nudge(bool left, bool up, bool right, bool down, float distance)
{
   // TODO this is not correct - misses framerate aware calculation -- distance probably comes via update
   Vector2 v { .x = (left ? -1.0f : 0.0f) + (right ? 1.0f : 0.0f), .y = (up ? -1.0f : 0.0f) + (down ? 1.0f : 0.0f) };
   auto normalized = Vector2Normalize(v);
   position = Vector2Add(position, Vector2Scale(normalized, distance));
}

MapCamera::Projection::Projection(Projection &&other) noexcept
{
   moveFrom(std::move(other));
}

MapCamera::Projection::Projection(Camera2D data)
{
   BeginMode2D(data);
   active = true;
}

MapCamera::Projection::~Projection()
{
   if (active)
   {
      EndMode2D();
   }
}

Vector2 const MapCamera::HOME_POSITION { .x = 0.0f, .y = 0.0f };

MapCamera::Projection &MapCamera::Projection::operator=(Projection &&other) noexcept
{
   moveFrom(std::move(other));
   return *this;
}

void MapCamera::Projection::moveFrom(Projection &&other) noexcept
{
   if (other.active)
   {
      active = true;
      other.active = false;
   }
}

MapCamera::MapCamera(std::shared_ptr<Gearbox> gearbox)
   : gearbox(std::move(gearbox))
{
   memset(&data, 0x00, sizeof(data));
}

void MapCamera::zoom(MapCamera::ZoomOperation const &op)
{
   gearbox->setTargetZoomFactor(op(gearbox->getTargetZoomFactor()));
}

void MapCamera::panTo(Vector2 target)
{
   gearbox->panTo(target);
}

void MapCamera::nudge(bool left, bool up, bool right, bool down)
{
   gearbox->nudge(left, up, right, down, 3.0f); // TODO: zoom-dependent distance?
}

MapCamera::Projection MapCamera::beginProjection(Vector2 viewportSize)
{
   Vector2 const &centerOfViewport = Vector2Scale(viewportSize, 0.5f);
   data.offset = centerOfViewport;
   data.target = gearbox->getCurrentPosition();
   data.zoom = gearbox->getCurrentZoomFactor().raw();
   return Projection { data };
}
