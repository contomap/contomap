#include "contomap/model/TopicName.h"

using contomap::model::TopicName;

TopicName::TopicName(std::string value)
   : value(std::move(value))
{
}

std::variant<std::monostate, TopicName> TopicName::from(std::string value)
{
   if (value.empty() || (value.length() > maxUtf8Bytes()))
   {
      return {};
   }
   return TopicName(std::move(value));
}
