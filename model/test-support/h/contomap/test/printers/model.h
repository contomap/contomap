#pragma once

#include <ostream>

#include "contomap/model/SpacialCoordinate.h"
#include "contomap/model/Topic.h"

namespace contomap::model
{

std::ostream &operator<<(std::ostream &stream, contomap::model::SpacialCoordinate const &spacial);
std::ostream &operator<<(std::ostream &stream, contomap::model::Topic const &topic);

}
