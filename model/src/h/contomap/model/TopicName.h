#pragma once

#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/OptionalIdentifier.h"
#include "contomap/model/TopicNameValue.h"

namespace contomap::model
{

/**
 * A TopicName represents the human interpretable identification of a topic.
 */
class TopicName
{
public:
   /**
    * Constructor.
    *
    * @param id the primary identifier of this name.
    * @param value the human readable name.
    */
   TopicName(contomap::model::Identifier id, contomap::model::TopicNameValue value);

   /**
    * @return the current value of this name.
    */
   [[nodiscard]] contomap::model::TopicNameValue getValue() const;

private:
   contomap::model::Identifier id;
   contomap::model::Identifiers scope;

   contomap::model::OptionalIdentifier reifier;

   contomap::model::TopicNameValue value;
};

}
