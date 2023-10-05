#include "contomap/frontend/RenderContext.h"

using contomap::frontend::RenderContext;

RenderContext::RenderContext(Vector2 dpiScale, Vector2 renderSize)
   : dpiScale(dpiScale)
   , renderSize(renderSize)
{
}

RenderContext RenderContext::fromCurrentState()
{
   return RenderContext(GetWindowScaleDPI(), Vector2 { .x = static_cast<float>(GetRenderWidth()), .y = static_cast<float>(GetRenderHeight()) });
}

Vector2 RenderContext::getContentSize() const
{
   return Vector2 { .x = renderSize.x / dpiScale.x, .y = renderSize.y / dpiScale.y };
}
