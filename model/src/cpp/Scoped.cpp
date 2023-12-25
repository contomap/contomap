#include "contomap/model/Scoped.h"
#include "contomap/model/Topic.h"

using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Scoped;
using contomap::model::Topic;

Scoped::Scoped(contomap::model::Identifiers topicIds)
   : topicIds(std::move(topicIds))
{
}

void Scoped::encodeScoped(contomap::infrastructure::serial::Encoder &coder) const
{
   topicIds.encode(coder, "scope");
}

void Scoped::decodeScoped(
   contomap::infrastructure::serial::Decoder &coder, std::function<contomap::model::Topic &(contomap::model::Identifier)> const &resolver)
{
   topicIds.decode(coder, "scope");
   for (auto id : topicIds)
   {
      resolver(id);
   }
}

bool Scoped::isIn(Identifiers const &thatScope) const
{
   return thatScope.contains(topicIds);
}

bool Scoped::scopeContains(Topic const &topic) const
{
   return topicIds.contains(topic.getId());
}
