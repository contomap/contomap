#pragma once

#include <memory>

#include "contomap/infrastructure/Referable.h"
#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/Coordinates.h"
#include "contomap/model/Identifiable.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/Reifiable.h"
#include "contomap/model/Scoped.h"
#include "contomap/model/Style.h"
#include "contomap/model/Styleable.h"
#include "contomap/model/Typeable.h"

namespace contomap::model
{

class Topic;

/**
 * An Occurrence represents the presence of a topic.
 */
class Occurrence : public contomap::infrastructure::Referable<Occurrence>,
                   public contomap::model::Identifiable,
                   public contomap::model::Reifiable<contomap::model::Topic>,
                   public contomap::model::Typeable,
                   public contomap::model::Scoped,
                   public contomap::model::Styleable
{
public:
   /**
    * Constructor.
    *
    * @param id the primary identifier of this occurrence.
    * @param topic the reference to the topic this occurrence represents.
    * @param scope the scope within which this occurrence is valid.
    * @param spacial the known, initial point where the occurrence is happening.
    */
   Occurrence(contomap::model::Identifier id, contomap::model::Topic &topic, contomap::model::Identifiers scope, contomap::model::SpacialCoordinate spacial);

   Occurrence &refine() override;

   /**
    * Deserializes the occurrence.
    *
    * @param coder the decoder to use.
    * @param version the version to consider.
    * @param id the primary identifier of this occurrence.
    * @param topic the topic this occurrence represents.
    * @param topicResolver the function to use for resolving topic references.
    * @return the decoded instance.
    */
   [[nodiscard]] static std::unique_ptr<Occurrence> from(contomap::infrastructure::serial::Decoder &coder, uint8_t version, contomap::model::Identifier id,
      Topic &topic, std::function<Topic &(contomap::model::Identifier)> const &topicResolver);

   /**
    * Serializes the occurrence.
    *
    * @param coder the encoder to use.
    */
   void encode(contomap::infrastructure::serial::Encoder &coder) const;

   [[nodiscard]] contomap::model::Identifier getId() const override;

   /**
    * @return the topic this occurrence represents.
    */
   [[nodiscard]] contomap::model::Topic &getTopic();
   /**
    * @return the topic this occurrence represents.
    */
   [[nodiscard]] contomap::model::Topic const &getTopic() const;

   /**
    * @return the location of this occurrence
    */
   [[nodiscard]] contomap::model::Coordinates const &getLocation() const;

   void setAppearance(contomap::model::Style style) override;
   [[nodiscard]] contomap::model::Style getAppearance() const override;

   /**
    * Move the location of the occurrence by given offset.
    *
    * @param offset the offset to apply.
    */
   void moveBy(contomap::model::SpacialCoordinate::Offset offset);

private:
   Occurrence(contomap::model::Identifier id, contomap::model::Topic &topic);

   contomap::model::Identifier id;
   contomap::model::Topic &topic;

   contomap::model::Coordinates location;
   contomap::model::Style appearance;
};

}
