#include "contomap/model/Coordinates.h"

using contomap::model::Coordinates;
using contomap::model::SpacialCoordinate;

void Coordinates::setSpacial(SpacialCoordinate value)
{
   spacial = value;
}

SpacialCoordinate Coordinates::getSpacial() const
{
   return spacial;
}
