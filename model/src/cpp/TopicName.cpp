#include <utility>

#include "contomap/model/TopicName.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::TopicName;
using contomap::model::TopicNameValue;

TopicName::TopicName(Identifier id)
   : id(id)
{
}

TopicName::TopicName(Identifier id, Identifiers scope, TopicNameValue value)
   : Scoped(std::move(scope))
   , id(id)
   , value(std::make_unique<TopicNameValue>(std::move(value)))
{
}

TopicName TopicName::from(contomap::infrastructure::serial::Decoder &coder, uint8_t, contomap::model::Identifier id,
   std::function<Topic &(contomap::model::Identifier)> const &topicResolver)
{
   Coder::Scope nameScope(coder, "topicName");
   TopicName name(id);
   name.decodeScoped(coder, topicResolver);
   name.value = std::make_unique<TopicNameValue>(TopicNameValue::from(coder));
   return name;
}

void TopicName::encode(Encoder &coder) const
{
   Coder::Scope nameScope(coder, "topicName");
   encodeScoped(coder);
   value->encode(coder);
}

Identifier TopicName::getId() const
{
   return id;
}

void TopicName::setValue(TopicNameValue newValue)
{
   *value = std::move(newValue);
}

TopicNameValue TopicName::getValue() const
{
   return *value;
}
