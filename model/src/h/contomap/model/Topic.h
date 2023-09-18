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
    * @return the unique identifier of this topic instance.
    */
   [[nodiscard]] contomap::model::Identifier getId() const;

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

   /**
    * @return a Search for all names in the topic.
    */
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::TopicName> allNames() const;

   /**
    * Return true if this instance has at least one occurrence that is in given scope.
    *
    * @param scope the scope to look for.
    * @return true if the topic is in given scope.
    */
   [[nodiscard]] bool isIn(contomap::model::Identifiers const &scope) const;

   /**
    * Return a Search for all occurrences that are in given scope.
    *
    * @param scope the scope to look for.
    * @return a Search matching the given scope.
    */
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Occurrence> occurrencesIn(contomap::model::Identifiers const &scope) const;

private:
   contomap::model::Identifier id;

   contomap::model::OptionalIdentifier reified;

   std::map<contomap::model::Identifier, contomap::model::TopicName> names;
   std::map<contomap::model::Identifier, contomap::model::Occurrence> occurrences;
   std::map<contomap::model::Identifier, contomap::model::Role> roles;
};

}
