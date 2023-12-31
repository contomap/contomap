#include <exception>

#include "contomap/model/Contomap.h"
#include "contomap/model/Filter.h"

using contomap::infrastructure::Search;
using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;
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
   auto it = associations.emplace(id, std::make_unique<Association>(id, std::move(scope), location));
   return *it.first->second;
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
   for (auto const &kvp : associations)
   {
      auto const &association = kvp.second;
      if (filter->matches(*association, *this))
      {
         co_yield *association;
      }
   }
}

std::optional<std::reference_wrapper<Association const>> Contomap::findAssociation(Identifier id) const
{
   auto it = associations.find(id);
   return (it != associations.end()) ? std::optional<std::reference_wrapper<Association const>>(*it->second)
                                     : std::optional<std::reference_wrapper<Association const>>();
}

std::optional<std::reference_wrapper<Association>> Contomap::findAssociation(Identifier id)
{
   auto it = associations.find(id);
   return (it != associations.end()) ? std::optional<std::reference_wrapper<Association>>(*it->second) : std::optional<std::reference_wrapper<Association>>();
}

contomap::infrastructure::Search<contomap::model::Occurrence const> Contomap::findOccurrences(Identifiers const &ids) const // NOLINT
{
   for (auto const &kvp : topics)
   {
      auto const &topic = kvp.second;
      for (auto &occurrence : topic->findOccurrences(ids))
      {
         co_yield occurrence;
      }
   }
}

contomap::infrastructure::Search<contomap::model::Occurrence> Contomap::findOccurrences(Identifiers const &ids) // NOLINT
{
   for (auto &kvp : topics)
   {
      auto &topic = kvp.second;
      for (auto &occurrence : topic->findOccurrences(ids))
      {
         co_yield occurrence;
      }
   }
}

contomap::infrastructure::Search<contomap::model::Role const> Contomap::findRoles(Identifiers const &ids) const // NOLINT
{
   for (auto const &kvp : topics)
   {
      auto const &topic = kvp.second;
      for (auto const &role : topic->findRoles(ids))
      {
         co_yield role;
      }
   }
}

contomap::infrastructure::Search<contomap::model::Role> Contomap::findRoles(Identifiers const &ids) // NOLINT
{
   for (auto &kvp : topics)
   {
      auto &topic = kvp.second;
      for (auto &role : topic->findRoles(ids))
      {
         co_yield role;
      }
   }
}

void Contomap::deleteRole(Identifier id)
{
   for (auto &[topicId, topic] : topics)
   {
      topic->removeRole(id);
   }
}

void Contomap::deleteAssociation(Identifier id)
{
   associations.erase(id);
}

void Contomap::deleteOccurrence(Identifier id)
{
   Identifiers topicsToDelete;
   for (auto &kvp : topics)
   {
      auto &topic = kvp.second;
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
   for (auto &kvp : associations)
   {
      auto &association = kvp.second;
      topic.removeRolesOf(*association);
      association->removeTopicReferences(topic.getId());
      if (association->isWithoutScope())
      {
         associationsToDelete.add(association->getId());
      }
   }
   deleteAssociations(associationsToDelete);

   for (auto &kvp : topics)
   {
      auto &otherTopic = kvp.second;
      otherTopic->removeTopicReferences(topic.getId());
      if (topicShouldBeRemoved(*otherTopic))
      {
         toDelete.add(otherTopic->getId());
      }
   }
}

void Contomap::encode(Encoder &coder) const
{
   Coder::Scope mapScope(coder, "contomap");
   coder.codeArray("topics", topics.begin(), topics.end(), [](Encoder &nested, auto const &kvp) {
      Coder::Scope nestedScope(nested, "");
      kvp.first.encode(nested, "id");
   });
   coder.codeArray("associations", associations.begin(), associations.end(), [](Encoder &nested, auto const &kvp) {
      Coder::Scope nestedScope(nested, "");
      kvp.first.encode(nested, "id");
      kvp.second->encodeProperties(nested);
   });
   coder.codeArray("topicRelated", topics.begin(), topics.end(), [](Encoder &nested, auto const &kvp) {
      Coder::Scope nestedScope(nested, "");
      kvp.first.encode(nested, "id");
      kvp.second->encodeRelated(nested);
   });

   defaultScope.encode(coder, "defaultScope");
}

void Contomap::decode(Decoder &coder, uint8_t version)
{
   associations.clear();
   topics.clear();

   Coder::Scope mapScope(coder, "contomap");
   coder.codeArray("topics", [this](Decoder &nested, size_t) {
      Coder::Scope nestedScope(nested, "");
      auto id = Identifier::from(nested, "id");
      topics.emplace(id, std::make_unique<Topic>(id));
   });
   auto topicResolver = [this](Identifier id) -> Topic & {
      auto it = topics.find(id);
      if (it == topics.end())
      {
         throw std::runtime_error("topic not found");
      }
      return *it->second;
   };
   coder.codeArray("associations", [this, version, &topicResolver](Decoder &nested, size_t) {
      Coder::Scope nestedScope(nested, "");
      auto id = Identifier::from(nested, "id");
      auto association = std::make_unique<Association>(id);
      association->decodeProperties(nested, version, topicResolver);
      associations.emplace(id, std::move(association));
   });
   auto associationResolver = [this](Identifier id) -> Association & {
      auto it = associations.find(id);
      if (it == associations.end())
      {
         throw std::runtime_error("association not found");
      }
      return *it->second;
   };
   coder.codeArray("topicRelated", [version, &topicResolver, &associationResolver](Decoder &nested, size_t) {
      Coder::Scope nestedScope(nested, "");
      Identifier topicId = Identifier::from(nested, "id");
      auto &topic = topicResolver(topicId);
      topic.decodeRelated(nested, version, topicResolver, associationResolver);
   });

   defaultScope = Identifier::from(coder, "defaultScope");
}
