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
   return { x, y };
}

SpacialCoordinate SpacialCoordinate::absoluteAt(CoordinateType x, CoordinateType y)
{
   SpacialCoordinate coordinate;
   coordinate.setAbsoluteReference(AbsolutePoint::at(x, y));
   return coordinate;
}

void SpacialCoordinate::setAbsoluteReference(AbsolutePoint point)
{
   absoluteReference = point;
}

SpacialCoordinate::AbsolutePoint SpacialCoordinate::getAbsoluteReference() const
{
   return absoluteReference;
}
