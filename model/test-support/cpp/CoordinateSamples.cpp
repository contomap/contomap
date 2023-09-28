#include "contomap/test/samples/CoordinateSamples.h"
#include "contomap/test/samples/RandomSamples.h"

using contomap::model::SpacialCoordinate;

using contomap::test::samples::randomSignedFraction;

namespace contomap::test::samples
{

SpacialCoordinate someSpacialCoordinate()
{
   return SpacialCoordinate::absoluteAt(randomSignedFraction() * 100.0f, randomSignedFraction() * 100.0f);
}

}
