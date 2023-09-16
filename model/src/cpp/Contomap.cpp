#include "contomap/model/Contomap.h"

using contomap::model::Contomap;
using contomap::model::Identifier;
using contomap::model::Topic;

Topic &Contomap::newTopic()
{
   auto id = Identifier::random();
   auto it = topics.emplace(id, Topic(id));
   return it.first->second;
}
