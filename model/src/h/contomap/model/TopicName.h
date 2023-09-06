#pragma once

#include <string>
#include <variant>

namespace contomap::model
{

/**
 * TopicName is the primary, human readable identifier of a topic.
 */
class TopicName
{
public:
   /**
    * @return the maximum length, in utf-8 bytes, of the name.
    */
   [[nodiscard]] static constexpr size_t maxUtf8Bytes()
   {
      return 200;
   }

   /**
    * Creates a new TopicName from the given raw string.
    * The string must not be empty and be shorter than the allowed maximum length.
    *
    * TODO replace return value with a better type
    *
    * @param value the raw string value to take over
    * @return either the right type of the created name, or an error instead.
    */
   [[nodiscard]] static std::variant<std::monostate, TopicName> from(std::string value);

   constexpr std::strong_ordering operator<=>(TopicName const &b) const noexcept = default;

private:
   explicit TopicName(std::string value);

   std::string value;
};

}
