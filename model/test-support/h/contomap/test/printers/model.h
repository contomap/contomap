#pragma once

#include <ostream>

#include "contomap/model/SpacialCoordinate.h"

namespace contomap::model
{

std::ostream &operator<<(std::ostream &stream, contomap::model::SpacialCoordinate const &spacial);

}
