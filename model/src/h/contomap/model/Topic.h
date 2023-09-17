#pragma once

#include <map>

#include "contomap/infrastructure/Generator.h"
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

   /**
    * Adds a new name to the topic.
    *
    * @param value the value of the name to add.
    * @return the created instance.
    */
   contomap::model::TopicName &newName(contomap::model::TopicNameValue value);

   /**
    * Adds a new occurrence to the topic.
    *
    * @param scope the scope within which the occurrence is valid.
    * @param location the location in the graphical map.
    * @return the created instance.
    */
   contomap::model::Occurrence &newOccurrence(contomap::model::Identifiers scope, contomap::model::SpacialCoordinate location);

   // using NameSearch = contomap::infrastructure::Generator<std::reference_wrapper<contomap::model::TopicName const>>;

   [[nodiscard]] contomap::infrastructure::Search<contomap::model::TopicName> allNames() const;

private:
   contomap::model::Identifier id;

   contomap::model::OptionalIdentifier reified;

   std::map<contomap::model::Identifier, contomap::model::TopicName> names;
   std::map<contomap::model::Identifier, contomap::model::Occurrence> occurrences;
   std::map<contomap::model::Identifier, contomap::model::Role> roles;
};

}
