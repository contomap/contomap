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

TopicName TopicName::from(contomap::infrastructure::serial::Decoder &decoder, uint8_t, contomap::model::Identifier id)
{
   Coder::Scope nameScope(decoder, "topicName");
   Identifiers scope;
   scope.decode(decoder, "scope");
   auto value = TopicNameValue::from(decoder);
   return { id, scope, value };
}

void TopicName::encode(Encoder &encoder) const
{
   Coder::Scope nameScope(encoder, "topicName");
   scope.encode(encoder, "scope");
   value.encode(encoder);
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
