#pragma once

#include <string>
#include <variant>

namespace contomap::model
{

/**
 * TopicName is the primary, human readable identifier of a topic.
 */
class TopicNameValue
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
   [[nodiscard]] static std::variant<std::monostate, TopicNameValue> from(std::string value);

   std::strong_ordering operator<=>(TopicNameValue const &b) const noexcept = default;

   /**
    * @return the raw text from this value.
    */
   [[nodiscard]] std::string raw() const;

private:
   explicit TopicNameValue(std::string value);

   std::string value;
};

}
