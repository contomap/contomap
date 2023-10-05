#pragma once

#include <gmock/gmock.h>

#include "contomap/model/Coordinates.h"

namespace contomap::test::matchers
{

MATCHER_P(isCloseTo, expected, "")
{
   auto const &expectedPoint = expected.getAbsoluteReference();
   auto const &argPoint = arg.getAbsoluteReference();
   return (std::abs(argPoint.X() - expectedPoint.X()) < 0.001f) && (std::abs(argPoint.Y() - expectedPoint.Y()) < 0.001f);
}

}
