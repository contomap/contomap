#include "contomap/model/SpacialCoordinate.h"

using contomap::model::SpacialCoordinate;

SpacialCoordinate::AbsolutePoint::AbsolutePoint()
   : SpacialCoordinate::AbsolutePoint(0.0, 0.0)
{
}

SpacialCoordinate::AbsolutePoint::AbsolutePoint(CoordinateType x, CoordinateType y)
   : x(x)
   , y(y)
{
}

SpacialCoordinate::AbsolutePoint SpacialCoordinate::AbsolutePoint::at(CoordinateType x, CoordinateType y)
{
   return AbsolutePoint(x, y);
}

void SpacialCoordinate::setAbsoluteReference(AbsolutePoint point)
{
   absoluteReference = point;
}

SpacialCoordinate::AbsolutePoint SpacialCoordinate::getAbsoluteReference() const
{
   return absoluteReference;
}
