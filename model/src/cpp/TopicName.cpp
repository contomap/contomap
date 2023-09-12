#include "contomap/model/TopicName.h"

using contomap::model::Identifier;
using contomap::model::TopicName;
using contomap::model::TopicNameValue;

TopicName::TopicName(Identifier id, TopicNameValue value)
   : id(id)
   , value(value)
{
}
