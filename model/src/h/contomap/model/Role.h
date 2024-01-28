#pragma once

#include <functional>
#include <memory>

#include "contomap/infrastructure/Link.h"
#include "contomap/infrastructure/Referable.h"
#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/Identifiable.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/Reifiable.h"
#include "contomap/model/Style.h"
#include "contomap/model/Styleable.h"
#include "contomap/model/Typeable.h"

namespace contomap::model
{

class Association;
class Topic;

/**
 * A Role represents the nature a topic plays in an association.
 */
class Role : public contomap::infrastructure::Referable<Role>,
             public contomap::model::Identifiable,
             public contomap::model::Reifiable<contomap::model::Topic>,
             public contomap::model::Typeable,
             public contomap::model::Styleable
{
public:
   /**
    * Constructor.
    *
    * @param id the unique identifier of this role.
    * @param topic the topic to represent in the association.
    * @param association the association the role is part of.
    */
   Role(contomap::model::Identifier id, contomap::model::Topic &topic, contomap::model::Association &association);

   Role &refine() override;

   /**
    * Deserializes the role.
    *
    * @param coder the decoder to use.
    * @param version the version to consider.
    * @param id the unique identifier of the role.
    * @param topicResolver the function to use for resolving topic references.
    * @param associationResolver the function to use for resolving association references.
    * @return the decoded role.
    */
   [[nodiscard]] static std::unique_ptr<Role> from(contomap::infrastructure::serial::Decoder &coder, uint8_t version, contomap::model::Identifier id,
      std::function<Topic &(contomap::model::Identifier)> const &topicResolver,
      std::function<Association &(contomap::model::Identifier)> const &associationResolver);

   /**
    * Serializes the role.
    *
    * @param coder the encoder to use.
    */
   void encode(contomap::infrastructure::serial::Encoder &coder) const;

   [[nodiscard]] Identifier getId() const override;

   /**
    * @return the primary identifier of the association this role is part of.
    */
   [[nodiscard]] Identifier getParent() const;

   void setAppearance(contomap::model::Style style) override;
   [[nodiscard]] contomap::model::Style getAppearance() const override;

private:
   void unlink();

   contomap::model::Identifier id;

   contomap::model::Style appearance;

   std::unique_ptr<contomap::infrastructure::Link<contomap::model::Topic>> topic;
   std::unique_ptr<contomap::infrastructure::Link<contomap::model::Association>> association;
};

}
