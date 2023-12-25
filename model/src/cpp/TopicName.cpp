#include <utility>

#include "contomap/model/TopicName.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::TopicName;
using contomap::model::TopicNameValue;

TopicName::TopicName(Identifier id, Identifiers scope, TopicNameValue value)
   : Scoped(std::move(scope))
   , id(id)
   , value(std::move(value))
{
}

TopicName TopicName::from(contomap::infrastructure::serial::Decoder &coder, uint8_t, contomap::model::Identifier id)
{
   Coder::Scope nameScope(coder, "topicName");
   Identifiers scope;
   scope.decode(coder, "scope");
   // TODO verify references -- prefer to use Scoped::decodeScoped()
   auto value = TopicNameValue::from(coder);
   return { id, scope, value };
}

void TopicName::encode(Encoder &coder) const
{
   Coder::Scope nameScope(coder, "topicName");
   encodeScoped(coder);
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
