#pragma once

#include "contomap/infrastructure/Link.h"
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
    * Establishes a link with given role.
    *
    * @param role the role instance to link with.
    * @param associationUnlinked the function to pass on to the returned link.
    * @return the link that refers to this instance.
    */
   [[nodiscard]] std::unique_ptr<contomap::infrastructure::Link<Association>> link(contomap::model::Role &role, std::function<void()> associationUnlinked);

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
   class RoleEntry
   {
   public:
      RoleEntry(std::unique_ptr<contomap::infrastructure::Link<contomap::model::Role>> link)
         : link(std::move(link))
      {
      }

      contomap::model::Role &role()
      {
         return link->getLinked();
      }

   private:
      std::unique_ptr<contomap::infrastructure::Link<contomap::model::Role>> link;
   };

   contomap::model::Identifier id;
   contomap::model::Identifiers scope;

   contomap::model::OptionalIdentifier type;

   contomap::model::Style appearance;
   contomap::model::Coordinates location;

   std::map<contomap::model::Identifier, std::unique_ptr<RoleEntry>> roles;
};

}
