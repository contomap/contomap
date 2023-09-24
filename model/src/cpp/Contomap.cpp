#include "contomap/model/Contomap.h"
#include "contomap/model/Filter.h"

using contomap::infrastructure::Search;
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

Search<Topic> Contomap::findTopics(std::shared_ptr<Filter<Topic>> filter) const
{
   for (auto const &[_, topic] : topics)
   {
      if (filter->matches(topic, *this))
      {
         co_yield topic;
      }
   }
}

std::optional<std::reference_wrapper<Topic const>> Contomap::findTopic(Identifier id) const
{
   auto it = topics.find(id);
   return (it != topics.end()) ? std::optional<std::reference_wrapper<Topic const>>(it->second) : std::optional<std::reference_wrapper<Topic const>>();
}

Search<Association> Contomap::findAssociations(std::shared_ptr<Filter<Association>> filter) const
{
   for (auto const &[_, association] : associations)
   {
      if (filter->matches(association, *this))
      {
         co_yield association;
      }
   }
}

std::optional<std::reference_wrapper<Association const>> Contomap::findAssociation(Identifier id) const
{
   auto it = associations.find(id);
   return (it != associations.end()) ? std::optional<std::reference_wrapper<Association const>>(it->second)
                                     : std::optional<std::reference_wrapper<Association const>>();
}
