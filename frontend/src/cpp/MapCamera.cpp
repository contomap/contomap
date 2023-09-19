#include <cstring>

#include <raylib.h>
#include <raymath.h>

#include "contomap/frontend/MapCamera.h"

using contomap::frontend::MapCamera;

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

MapCamera::MapCamera()
{
   memset(&data, 0x00, sizeof(data));
   data.zoom = 1.0f;
}

MapCamera::Projection MapCamera::beginProjection(Vector2 viewportSize)
{
   data.offset = Vector2Scale(viewportSize, 0.5f);
   return Projection { data };
}
