#include "contomap/model/Coordinates.h"

using contomap::model::Coordinates;
using contomap::model::SpacialCoordinate;

Coordinates::Coordinates(SpacialCoordinate spacial)
   : spacial(spacial)
{
}

void Coordinates::setSpacial(SpacialCoordinate value)
{
   spacial = value;
}

SpacialCoordinate Coordinates::getSpacial() const
{
   return spacial;
}
