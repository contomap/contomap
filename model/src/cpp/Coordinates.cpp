#include "contomap/model/Coordinates.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Coordinates;
using contomap::model::SpacialCoordinate;

Coordinates::Coordinates(SpacialCoordinate spacial)
   : spacial(spacial)
{
}

void Coordinates::encode(Encoder &coder, std::string const &name) const
{
   Coder::Scope scope(coder, name);
   spacial.encode(coder, "spacial");
}

void Coordinates::decode(Decoder &coder, std::string const &name, uint8_t version)
{
   Coder::Scope scope(coder, name);
   spacial.decode(coder, "spacial", version);
}

void Coordinates::setSpacial(SpacialCoordinate value)
{
   spacial = value;
}

SpacialCoordinate Coordinates::getSpacial() const
{
   return spacial;
}
