#pragma once

#include "contomap/model/Coordinates.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/OptionalIdentifier.h"
#include "contomap/model/Role.h"

namespace contomap::model
{

/**
 * An Association represents the link or relation between topics.
 */
class Association
{
public:
   /**
    * Constructor.
    *
    * @param id the primary identifier of this association.
    * @param scope the scope within which this association is valid.
    * @param spacial the known, initial point where the association is happening.
    */
   Association(contomap::model::Identifier id, contomap::model::Identifiers scope, contomap::model::SpacialCoordinate spacial);

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

   void removeTopicReferences(contomap::model::Identifier topicId);

private:
   contomap::model::Identifier id;
   contomap::model::Identifiers scope;

   contomap::model::OptionalIdentifier reifier;
   contomap::model::OptionalIdentifier type;

   contomap::model::OptionalIdentifier appearance;
   contomap::model::Coordinates location;

   contomap::model::Identifiers roles;
};

}
