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
   : zoomFactor(ZoomFactor::UNIT)
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

void MapCamera::zoomNearer()
{
   // TODO add limits -- if applicable with the intended zoom(operation) variant.
   zoomLevel += 5;
   gearbox->setTargetZoomFactor(ZoomFactor::from(std::pow(2.0f, static_cast<float>(zoomLevel) / 10.0f)));
}

void MapCamera::zoomFarther()
{
   zoomLevel -= 5;
   gearbox->setTargetZoomFactor(ZoomFactor::from(std::pow(2.0f, static_cast<float>(zoomLevel) / 10.0f)));
}

MapCamera::Projection MapCamera::beginProjection(Vector2 viewportSize)
{
   data.offset = Vector2Scale(viewportSize, 0.5f);
   data.zoom = gearbox->getCurrentZoomFactor().raw();
   return Projection { data };
}
