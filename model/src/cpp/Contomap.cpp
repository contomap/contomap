#include "contomap/model/Contomap.h"
#include "contomap/model/TopicFilter.h"

using contomap::model::Contomap;
using contomap::model::Identifier;
using contomap::model::Topic;

Topic &Contomap::newTopic()
{
   auto id = Identifier::random();
   auto it = topics.emplace(id, Topic(id));
   return it.first->second;
}

Contomap::Search Contomap::findTopics(std::shared_ptr<TopicFilter> filter) const
{
   for (auto const &[_, topic] : topics)
   {
      if (filter->matches(topic, *this))
      {
         co_yield topic;
      }
   }
}
