#pragma once

#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/OptionalIdentifier.h"
#include "contomap/model/TopicNameValue.h"

namespace contomap::model
{

/**
 * A Style contains information on how to present things.
 */
class Style
{
public:
   /**
    * Constructor.
    *
    * @param id the primary identifier of this style.
    */
   explicit Style(contomap::model::Identifier id);

private:
   contomap::model::Identifier id;
   contomap::model::OptionalIdentifier localTo;
};

}
