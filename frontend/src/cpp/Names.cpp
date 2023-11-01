#include "contomap/frontend/Names.h"

using contomap::frontend::Names;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Topic;
using contomap::model::TopicName;

std::vector<std::string> Names::forDisplay(Topic const &topic, Identifier defaultScope)
{
   std::vector<std::string> result;

   if (topic.getId() == defaultScope)
   {
      result.emplace_back("---");
   }
   for (TopicName const &name : topic.allNames())
   {
      result.push_back(name.getValue().raw());
   }
   if (result.empty())
   {
      result.emplace_back("???");
   }

   return result;
}
