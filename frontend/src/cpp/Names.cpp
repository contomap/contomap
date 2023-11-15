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

std::vector<std::string> Names::forScopedDisplay(Topic const &topic, Identifiers const &scope, Identifier defaultScope)
{
   std::vector<std::string> result;

   if (topic.getId() == defaultScope)
   {
      result.emplace_back("---");
   }
   std::vector<std::reference_wrapper<TopicName const>> namesInScope;
   for (TopicName const &name : topic.allNames())
   {
      if (name.isIn(scope))
      {
         namesInScope.emplace_back(name);
      }
   }
   if (!namesInScope.empty())
   {
      std::sort(namesInScope.begin(), namesInScope.end(), [](TopicName const &a, TopicName const &b) { return a.hasNarrowerScopeThan(b); });
      TopicName const &referenceName = namesInScope[0];
      for (TopicName const &name : namesInScope)
      {
         if (name.hasSameScopeSizeAs(referenceName))
         {
            result.emplace_back(name.getValue().raw());
         }
      }
   }
   else
   {
      result.emplace_back("???");
   }

   return result;
}
