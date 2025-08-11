#pragma once

#include <memory>

#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/Identifiable.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/Scoped.h"
#include "contomap/model/TopicNameValue.h"

namespace contomap::model
{

class Topic;

/**
 * A TopicName represents the human interpretable identification of a topic.
 */
class TopicName : public contomap::model::Scoped, public contomap::model::Identifiable
{
public:
   /**
    * Constructor.
    *
    * @param id the primary identifier of this name.
    * @param scope the scope within which this name is valid.
    * @param value the human readable name.
    */
   TopicName(contomap::model::Identifier id, contomap::model::Identifiers scope, contomap::model::TopicNameValue value);

   /**
    * Deserialize the topic name value.
    *
    * @param coder the decoder to use.
    * @param version the version to consider.
    * @param id the identifier of the instance.
    * @param topicResolver the resolver to use for topics.
    * @return the decoded instance
    */
   [[nodiscard]] static TopicName from(contomap::infrastructure::serial::Decoder &coder, uint8_t version, contomap::model::Identifier id,
      std::function<contomap::model::Topic &(contomap::model::Identifier)> const &topicResolver);

   /**
    * Serialize the topic name.
    *
    * @param coder the encoder to use.
    */
   void encode(contomap::infrastructure::serial::Encoder &coder) const;

   [[nodiscard]] contomap::model::Identifier getId() const override;

   /**
    * Sets the new value of the name.
    *
    * @param newValue the human readable name.
    */
   void setValue(contomap::model::TopicNameValue newValue);

   /**
    * @return the current value of this name.
    */
   [[nodiscard]] contomap::model::TopicNameValue getValue() const;

private:
   explicit TopicName(contomap::model::Identifier id);

   contomap::model::Identifier id;
   std::unique_ptr<contomap::model::TopicNameValue> value;
};

}
