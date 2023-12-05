#pragma once

#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/Coordinates.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/OptionalIdentifier.h"
#include "contomap/model/Reifiable.h"
#include "contomap/model/Role.h"
#include "contomap/model/Style.h"

namespace contomap::model
{

class Topic;

/**
 * An Association represents the link or relation between topics.
 */
class Association : public contomap::model::Reifiable<contomap::model::Topic>
{
public:
   /**
    * Constructor.
    *
    * @param id the primary identifier of this association.
    */
   explicit Association(contomap::model::Identifier id);
   /**
    * Constructor.
    *
    * @param id the primary identifier of this association.
    * @param scope the scope within which this association is valid.
    * @param spacial the known, initial point where the association is happening.
    */
   Association(contomap::model::Identifier id, contomap::model::Identifiers scope, contomap::model::SpacialCoordinate spacial);

   /**
    * Serializes the properties of the association.
    *
    * @param coder the coder to use.
    */
   void encodeProperties(contomap::infrastructure::serial::Encoder &coder) const;

   /**
    * Deserializes the properties of the association.
    *
    * @param coder the coder to use.
    * @param version the version to consider.
    */
   void decodeProperties(contomap::infrastructure::serial::Decoder &coder, uint8_t version);

   /**
    * @return the unique identifier of this association instance.
    */
   [[nodiscard]] contomap::model::Identifier getId() const;

   /**
    * @return the location of this association
    */
   [[nodiscard]] contomap::model::Coordinates const &getLocation() const;

   /**
    * Moves the association to the specified position.
    *
    * @param absolutePosition the new location.
    */
   void moveTo(SpacialCoordinate absolutePosition);

   /**
    * Return true if this instance is in the given scope.
    *
    * @param thatScope the scope to look for.
    * @return true if the association is in given scope.
    */
   [[nodiscard]] bool isIn(contomap::model::Identifiers const &thatScope) const;

   /**
    * @return true if the association is nowhere presented.
    */
   [[nodiscard]] bool isWithoutScope() const;

   /**
    * Adds a new role to the association.
    *
    * @return the created role details. Use the result as a template for storing the concrete instance somewhere.
    */
   [[nodiscard]] Role::Seed addRole();

   /**
    * Removes given role from the association.
    *
    * @param role the role to remove.
    * @return true if the role was part of this association.
    */
   bool removeRole(contomap::model::Role const &role);

   /**
    * @param roleId the identifier to check.
    * @return true if the association has the role with given identifier.
    */
   [[nodiscard]] bool hasRole(contomap::model::Identifier roleId) const;

   /**
    * @return true if the association has at least one role.
    */
   [[nodiscard]] bool hasRoles() const;

   /**
    * Remove any references this association might haven to this topic.
    *
    * @param topicId the identifier of the topic to clear.
    */
   void removeTopicReferences(contomap::model::Identifier topicId);

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
   contomap::model::Identifiers scope;

   contomap::model::OptionalIdentifier type;

   contomap::model::Style appearance;
   contomap::model::Coordinates location;

   contomap::model::Identifiers roles;
};

}
