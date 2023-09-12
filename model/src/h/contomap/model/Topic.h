#pragma once

#include <map>

#include "contomap/model/Identifier.h"
#include "contomap/model/Occurrence.h"
#include "contomap/model/Role.h"
#include "contomap/model/TopicName.h"

namespace contomap::model
{

/**
 * A Topic captures the information about a particular subject.
 */
class Topic
{
public:
   /**
    * Constructor.
    *
    * @param id the primary identifier of this name.
    * @param value the human readable name.
    */
   explicit Topic(contomap::model::Identifier id);

private:
   contomap::model::Identifier id;

   contomap::model::OptionalIdentifier reified;

   std::map<contomap::model::Identifier, TopicName> names;
   std::map<contomap::model::Identifier, Occurrence> occurrences;
   std::map<contomap::model::Identifier, Role> roles;
};

}
