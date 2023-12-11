#include <utility>

#include "contomap/model/TopicName.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Encoder;
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

TopicName TopicName::from(contomap::infrastructure::serial::Decoder &coder, uint8_t, contomap::model::Identifier id)
{
   Coder::Scope nameScope(coder, "topicName");
   Identifiers scope;
   scope.decode(coder, "scope");
   auto value = TopicNameValue::from(coder);
   return { id, scope, value };
}

void TopicName::encode(Encoder &coder) const
{
   Coder::Scope nameScope(coder, "topicName");
   scope.encode(coder, "scope");
   value.encode(coder);
}

Identifier TopicName::getId() const
{
   return id;
}

void TopicName::setValue(TopicNameValue newValue)
{
   value = std::move(newValue);
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

bool TopicName::scopeEquals(Identifiers const &thatScope) const
{
   return scope == thatScope;
}

bool TopicName::hasNarrowerScopeThan(TopicName const &other) const
{
   return (scope.size() > other.scope.size()) || (hasSameScopeSizeAs(other) && (scope < other.scope));
}

bool TopicName::hasSameScopeSizeAs(TopicName const &other) const
{
   return scope.size() == other.scope.size();
}
