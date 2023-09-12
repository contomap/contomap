#pragma once

#include <map>

#include "contomap/model/Association.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Style.h"
#include "contomap/model/Topic.h"

namespace contomap::model
{

/**
 * Contomap is the overall type to contain all the information.
 */
class Contomap
{
public:
private:
   std::map<contomap::model::Identifier, contomap::model::Topic> topics;
   std::map<contomap::model::Identifier, contomap::model::Association> associations;
   std::map<contomap::model::Identifier, contomap::model::Style> styles;
};

}
