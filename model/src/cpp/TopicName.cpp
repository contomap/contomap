#include <utility>

#include "contomap/model/TopicName.h"

using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::TopicName;
using contomap::model::TopicNameValue;

TopicName::TopicName(Identifier id, Identifiers scope, TopicNameValue value)
   : id(id)
   , scope(std::move(scope))
   , value(std::move(value))
{
}

TopicNameValue TopicName::getValue() const
{
   return value;
}

bool TopicName::isIn(Identifiers const &thatScope) const
{
   return thatScope.contains(scope);
}

bool TopicName::scopeContains(contomap::model::Identifier thatId) const
{
   return scope.contains(thatId);
}

bool TopicName::hasNarrowerScopeThan(TopicName const &other) const
{
   return (scope.size() > other.scope.size()) || (hasSameScopeSizeAs(other) && (scope < other.scope));
}

bool TopicName::hasSameScopeSizeAs(TopicName const &other) const
{
   return scope.size() == other.scope.size();
}
