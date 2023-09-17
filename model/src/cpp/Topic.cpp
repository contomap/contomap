#include "contomap/model/Topic.h"

using contomap::infrastructure::Search;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::SpacialCoordinate;
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

Search<TopicName> Topic::allNames() const
{
   for (auto const &[_, name] : names)
   {
      co_yield name;
   }
}

Occurrence &Topic::newOccurrence(Identifiers scope, SpacialCoordinate location)
{
   auto occurrenceId = Identifier::random();
   auto it = occurrences.emplace(occurrenceId, Occurrence(id, std::move(scope), location));
   return it.first->second;
}
