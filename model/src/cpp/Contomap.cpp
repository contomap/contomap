#include "contomap/model/Contomap.h"
#include "contomap/model/Filter.h"

using contomap::infrastructure::Search;
using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Association;
using contomap::model::Contomap;
using contomap::model::Identifier;
using contomap::model::Topic;

Contomap::Contomap()
   : defaultScope(Identifier::random())
{
   topics.emplace(defaultScope, std::make_unique<Topic>(defaultScope));
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
   auto it = topics.emplace(id, std::make_unique<Topic>(id));
   return *it.first->second;
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
      if (filter->matches(*it.second, *this))
      {
         co_yield *it.second;
      }
   }
}

contomap::infrastructure::Search<contomap::model::Topic> Contomap::find(std::shared_ptr<contomap::model::Filter<contomap::model::Topic>> filter) // NOLINT
{
   for (auto &it : topics)
   {
      if (filter->matches(*it.second, *this))
      {
         co_yield *it.second;
      }
   }
}

std::optional<std::reference_wrapper<Topic const>> Contomap::findTopic(Identifier id) const
{
   auto it = topics.find(id);
   return (it != topics.end()) ? std::optional<std::reference_wrapper<Topic const>>(*it->second) : std::optional<std::reference_wrapper<Topic const>>();
}

std::optional<std::reference_wrapper<Topic>> Contomap::findTopic(Identifier id)
{
   auto it = topics.find(id);
   return (it != topics.end()) ? std::optional<std::reference_wrapper<Topic>>(*it->second) : std::optional<std::reference_wrapper<Topic>>();
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

contomap::infrastructure::Search<contomap::model::Occurrence const> Contomap::findOccurrences(Identifiers const &ids) const // NOLINT
{
   for (auto const &[_, topic] : topics)
   {
      for (auto &occurrence : topic->findOccurrences(ids))
      {
         co_yield occurrence;
      }
   }
}

contomap::infrastructure::Search<contomap::model::Occurrence> Contomap::findOccurrences(Identifiers const &ids) // NOLINT
{
   for (auto &[_, topic] : topics)
   {
      for (auto &occurrence : topic->findOccurrences(ids))
      {
         co_yield occurrence;
      }
   }
}

contomap::infrastructure::Search<contomap::model::Role const> Contomap::findRoles(Identifiers const &ids) const // NOLINT
{
   for (auto const &[_, topic] : topics)
   {
      for (auto const &role : topic->findRoles(ids))
      {
         co_yield role;
      }
   }
}

contomap::infrastructure::Search<contomap::model::Role> Contomap::findRoles(Identifiers const &ids) // NOLINT
{
   for (auto &[_, topic] : topics)
   {
      for (auto &role : topic->findRoles(ids))
      {
         co_yield role;
      }
   }
}

void Contomap::deleteRole(Identifier id)
{
   for (auto &[associationId, association] : associations)
   {
      if (association.hasRole(id))
      {
         for (auto &[topicId, topic] : topics)
         {
            topic->removeRole(association, id);
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
      topic->removeRolesOf(association);
   }
   associations.erase(id);
}

void Contomap::deleteOccurrence(Identifier id)
{
   Identifiers topicsToDelete;
   for (auto &[_, topic] : topics)
   {
      if (topic->removeOccurrence(id) && topicShouldBeRemoved(*topic))
      {
         topicsToDelete.add(topic->getId());
      }
   }
   deleteTopicsCascading(topicsToDelete);
}

bool Contomap::topicShouldBeRemoved(Topic const &topic)
{
   return topic.isWithoutOccurrences() && (topic.getId() != defaultScope);
}

void Contomap::deleteTopicsCascading(Identifiers toDelete)
{
   while (!toDelete.empty())
   {
      Identifiers localToDelete = toDelete;
      toDelete.clear();
      for (auto const &topicId : localToDelete)
      {
         auto it = topics.find(topicId);
         if (it != topics.end())
         {
            deleting(toDelete, *it->second);
            topics.erase(it);
         }
      }
   }
}

void Contomap::deleting(Identifiers &toDelete, Topic &topic)
{
   Identifiers associationsToDelete;
   for (auto &[_, association] : associations)
   {
      topic.removeRolesOf(association);
      association.removeTopicReferences(topic.getId());
      if (association.isWithoutScope())
      {
         associationsToDelete.add(association.getId());
      }
   }
   deleteAssociations(associationsToDelete);

   for (auto &[_, otherTopic] : topics)
   {
      otherTopic->removeTopicReferences(topic.getId());
      if (topicShouldBeRemoved(*otherTopic))
      {
         toDelete.add(otherTopic->getId());
      }
   }
}

void Contomap::encode(Encoder &encoder, uint8_t version) const
{
   encoder.codeArray("topics", topics.begin(), topics.end(), [version](Encoder &nested, auto const &kvp) {
      Coder::Scope scope(nested, "");
      kvp.first.encode(nested, "id");
      kvp.second->encodeProperties(nested, version);
   });
}
