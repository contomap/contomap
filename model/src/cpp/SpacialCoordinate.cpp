#include "contomap/model/SpacialCoordinate.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
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

SpacialCoordinate::AbsolutePoint SpacialCoordinate::AbsolutePoint::from(Decoder &coder, std::string const &name)
{
   Coder::Scope scope(coder, name);
   float x = 0.0f;
   float y = 0.0f;
   coder.code("x", x);
   coder.code("y", y);
   return at(x, y);
}

void SpacialCoordinate::AbsolutePoint::encode(Encoder &coder, std::string const &name) const
{
   Coder::Scope scope(coder, name);
   coder.code("x", x);
   coder.code("y", y);
}

SpacialCoordinate SpacialCoordinate::absoluteAt(CoordinateType x, CoordinateType y)
{
   SpacialCoordinate coordinate;
   coordinate.setAbsoluteReference(AbsolutePoint::at(x, y));
   return coordinate;
}

void SpacialCoordinate::encode(contomap::infrastructure::serial::Encoder &coder, std::string const &name) const
{
   Coder::Scope scope(coder, name);
   absoluteReference.encode(coder, "absoluteReference");
}

void SpacialCoordinate::decode(Decoder &coder, std::string const &name, uint8_t)
{
   Coder::Scope scope(coder, name);
   absoluteReference = AbsolutePoint::from(coder, "absoluteReference");
}

void SpacialCoordinate::setAbsoluteReference(AbsolutePoint point)
{
   absoluteReference = point;
}

SpacialCoordinate::AbsolutePoint SpacialCoordinate::getAbsoluteReference() const
{
   return absoluteReference;
}
