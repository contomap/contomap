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

std::string TopicNameValue::raw() const
{
   return value;
}
