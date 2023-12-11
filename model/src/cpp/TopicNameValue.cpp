#include <stdexcept>

#include "contomap/model/TopicNameValue.h"

using contomap::model::TopicNameValue;

TopicNameValue::TopicNameValue(std::string value)
   : value(std::move(value))
{
}

std::variant<std::monostate, TopicNameValue> TopicNameValue::from(std::string value)
{
   if (value.empty() || (value.length() > maxUtf8Bytes()))
   {
      return {};
   }
   return TopicNameValue(std::move(value));
}

TopicNameValue TopicNameValue::from(contomap::infrastructure::serial::Decoder &coder)
{
   std::string value;
   coder.code("value", value);
   auto result = from(value);
   if (!std::holds_alternative<TopicNameValue>(result))
   {
      throw std::runtime_error("invalid name stored");
   }
   return std::get<TopicNameValue>(result);
}

void TopicNameValue::encode(contomap::infrastructure::serial::Encoder &coder) const
{
   coder.code("value", value);
}

std::string TopicNameValue::raw() const
{
   return value;
}
