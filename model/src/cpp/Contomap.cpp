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

Search<Topic const> Contomap::find(std::shared_ptr<Filter<Topic>> filter) const
{
   for (auto const &it : topics)
   {
      if (filter->matches(it.second, *this))
      {
         co_yield it.second;
      }
   }
}

Search<Topic> Contomap::find(std::shared_ptr<Filter<contomap::model::Topic>> filter)
{
   for (auto &it : topics)
   {
      if (filter->matches(it.second, *this))
      {
         co_yield it.second;
      }
   }
}

std::optional<std::reference_wrapper<Topic const>> Contomap::findTopic(Identifier id) const
{
   auto it = topics.find(id);
   return (it != topics.end()) ? std::optional<std::reference_wrapper<Topic const>>(it->second) : std::optional<std::reference_wrapper<Topic const>>();
}

Search<Association const> Contomap::find(std::shared_ptr<Filter<Association>> filter) const
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

std::optional<std::reference_wrapper<Association>> Contomap::findAssociation(Identifier id)
{
   auto it = associations.find(id);
   return (it != associations.end()) ? std::optional<std::reference_wrapper<Association>>(it->second) : std::optional<std::reference_wrapper<Association>>();
}
