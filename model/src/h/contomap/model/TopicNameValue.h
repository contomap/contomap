#pragma once

#include <string>
#include <variant>

#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"

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
    * @param value the raw string value to take over
    * @return either the right type of the created name, or an error instead.
    */
   [[nodiscard]] static std::variant<std::monostate, TopicNameValue> from(std::string value);

   /**
    * Deserialize the topic name value.
    *
    * @param coder the decoder to use.
    * @return the decoded instance
    */
   [[nodiscard]] static TopicNameValue from(contomap::infrastructure::serial::Decoder &coder);

   /**
    * Serialize the topic name value.
    *
    * @param coder the encoder to use.
    */
   void encode(contomap::infrastructure::serial::Encoder &coder) const;

   /**
    * Spaceship operator.
    *
    * @param other the other instance to compare to.
    * @return the ordering for this type.
    */
   std::strong_ordering operator<=>(TopicNameValue const &other) const noexcept = default;

   /**
    * @return the raw text from this value.
    */
   [[nodiscard]] std::string raw() const;

private:
   explicit TopicNameValue(std::string value);

   std::string value;
};

}
