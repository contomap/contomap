#pragma once

#include "contomap/model/Coordinates.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/OptionalIdentifier.h"
#include "contomap/model/Style.h"

namespace contomap::model
{

/**
 * An Occurrence represents the presence of a topic.
 */
class Occurrence
{
public:
   /**
    * Constructor.
    *
    * @param id the primary identifier of this occurrence.
    * @param topicId the primary identifier of the topic this occurrence represents.
    * @param scope the scope within which this occurrence is valid.
    * @param spacial the known, initial point where the occurrence is happening.
    */
   Occurrence(
      contomap::model::Identifier id, contomap::model::Identifier topicId, contomap::model::Identifiers scope, contomap::model::SpacialCoordinate spacial);

   /**
    * @return the unique identifier of this occurrence instance.
    */
   [[nodiscard]] contomap::model::Identifier getId() const;

   /**
    * @return the unique identifier of the topic this occurrence represents.
    */
   [[nodiscard]] contomap::model::Identifier getTopicId() const;

   /**
    * @return the scope of this occurrence.
    */
   [[nodiscard]] contomap::model::Identifiers const &getScope() const;

   /**
    * @return the location of this occurrence
    */
   [[nodiscard]] contomap::model::Coordinates const &getLocation() const;

   /**
    * Return true if this instance is in the given scope.
    *
    * @param thatScope the scope to look for.
    * @return true if the occurrence is in given scope.
    */
   [[nodiscard]] bool isIn(contomap::model::Identifiers const &thatScope) const;

   /**
    * Return true if the scope contains the given identifier.
    *
    * @param thatId the identifier to check .
    * @return true in case the identifier is part of the scope.
    */
   [[nodiscard]] bool scopeContains(contomap::model::Identifier thatId) const;

   /**
    * Set the style of the appearance.
    *
    * @param style the new style to set.
    */
   void setAppearance(contomap::model::Style style);
   /**
    * @return the current style of the appearance.
    */
   [[nodiscard]] contomap::model::Style getAppearance() const;

   /**
    * Assign the type of this occurrence.
    *
    * @param typeTopicId the identifier of the topic that describes this occurrence.
    */
   void setType(contomap::model::Identifier typeTopicId);
   /**
    * Clears the type of this occurrence.
    */
   void clearType();
   /**
    * @return the identifier of the topic that describes this occurrence, if set.
    */
   [[nodiscard]] contomap::model::OptionalIdentifier getType() const;

private:
   contomap::model::Identifier id;
   contomap::model::Identifier topicId;
   contomap::model::Identifiers scope;

   contomap::model::OptionalIdentifier reifier;
   contomap::model::OptionalIdentifier type;

   contomap::model::Style appearance;
   contomap::model::Coordinates location;
};

}
