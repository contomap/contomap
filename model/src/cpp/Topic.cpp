#include "contomap/model/Topic.h"

using contomap::model::Identifier;
using contomap::model::Topic;
using contomap::model::TopicName;

Topic::Topic(Identifier id)
   : id(id)
{
}

TopicName &Topic::newName(contomap::model::TopicNameValue value)
{
   auto nameId = Identifier::random();
   auto it = names.emplace(nameId, TopicName(nameId, std::move(value)));
   return it.first->second;
}
