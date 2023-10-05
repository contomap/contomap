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

void Contomap::deleteRoles(Identifiers const &ids)
{
   std::for_each(ids.begin(), ids.end(), [this](Identifier id) { deleteRole(id); });
}

void Contomap::deleteAssociations(Identifiers const &ids)
{
   std::for_each(ids.begin(), ids.end(), [this](Identifier id) { deleteAssociation(id); });
}

void Contomap::deleteOccurrences(Identifiers const &ids)
{
   std::for_each(ids.begin(), ids.end(), [this](Identifier id) { deleteOccurrence(id); });
}

contomap::infrastructure::Search<contomap::model::Topic const> Contomap::find( // NOLINT
   std::shared_ptr<contomap::model::Filter<contomap::model::Topic>> filter) const
{
   for (auto const &it : topics)
   {
      if (filter->matches(it.second, *this))
      {
         co_yield it.second;
      }
   }
}

contomap::infrastructure::Search<contomap::model::Topic> Contomap::find(std::shared_ptr<contomap::model::Filter<contomap::model::Topic>> filter) // NOLINT
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

contomap::infrastructure::Search<contomap::model::Association const> Contomap::find( // NOLINT
   std::shared_ptr<contomap::model::Filter<contomap::model::Association>> filter) const
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

void Contomap::deleteRole(Identifier id)
{
   for (auto &[associationId, association] : associations)
   {
      if (association.hasRole(id))
      {
         for (auto &[topicId, topic] : topics)
         {
            topic.removeRole(association, id);
         }
      }
   }
}

void Contomap::deleteAssociation(Identifier id)
{
   if (!associations.contains(id))
   {
      return;
   }
   auto &association = associations.at(id);
   for (auto &[_, topic] : topics)
   {
      topic.removeRolesOf(association);
   }
   associations.erase(id);
}

void Contomap::deleteOccurrence(Identifier id)
{
   for (auto it = topics.begin(); it != topics.end();)
   {
      Topic &topic = it->second;

      if (topic.removeOccurrence(id) && topicShouldBeRemoved(topic))
      {
         deleting(topic);
         it = topics.erase(it);
      }
      else
      {
         ++it;
      }
   }
}

bool Contomap::topicShouldBeRemoved(Topic const &topic)
{
   return topic.isWithoutOccurrences();
}

void Contomap::deleting(Topic &topic)
{
   // This is not the best algorithm, to try out all the associations, instead of iterating over the existing roles.
   for (auto &[_, association] : associations)
   {
      topic.removeRolesOf(association);
   }
}
