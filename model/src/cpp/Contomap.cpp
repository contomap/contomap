#include "contomap/model/Contomap.h"
#include "contomap/model/TopicFilter.h"

using contomap::model::Association;
using contomap::model::Contomap;
using contomap::model::Identifier;
using contomap::model::Topic;

Contomap::Contomap()
   : defaultScope(Identifier::random())
{
   topics.emplace(defaultScope, Topic(defaultScope));
}

Contomap Contomap::newMap()
{
   return {};
}

Identifier Contomap::getDefaultScope() const
{
   return defaultScope;
}

Topic &Contomap::newTopic()
{
   auto id = Identifier::random();
   auto it = topics.emplace(id, Topic(id));
   return it.first->second;
}

Association &Contomap::newAssociation(Identifiers scope, SpacialCoordinate location)
{
   auto id = Identifier::random();
   auto it = associations.emplace(id, Association(id, std::move(scope), location));
   return it.first->second;
}

contomap::infrastructure::Search<Topic> Contomap::findTopics(std::shared_ptr<TopicFilter> filter) const
{
   for (auto const &[_, topic] : topics)
   {
      if (filter->matches(topic, *this))
      {
         co_yield topic;
      }
   }
}

std::optional<std::reference_wrapper<Topic const>> Contomap::findTopic(contomap::model::Identifier id) const
{
   auto it = topics.find(id);
   return (it != topics.end()) ? std::optional<std::reference_wrapper<Topic const>>(it->second) : std::optional<std::reference_wrapper<Topic const>>();
}
