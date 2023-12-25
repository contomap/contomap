#pragma once

#include "contomap/infrastructure/Link.h"
#include "contomap/infrastructure/Referable.h"
#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/Coordinates.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/Reifiable.h"
#include "contomap/model/Role.h"
#include "contomap/model/Scoped.h"
#include "contomap/model/Style.h"
#include "contomap/model/Styleable.h"
#include "contomap/model/Typeable.h"

namespace contomap::model
{

class Topic;

/**
 * An Association represents the link or relation between topics.
 */
class Association : public contomap::infrastructure::Referable<Association>,
                    public contomap::model::Reifiable<contomap::model::Topic>,
                    public contomap::model::Typeable,
                    public contomap::model::Scoped,
                    public contomap::model::Styleable
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

   Association &refine() override;

   /**
    * Serializes the properties of the association.
    *
    * @param coder the encoder to use.
    */
   void encodeProperties(contomap::infrastructure::serial::Encoder &coder) const;

   /**
    * Deserializes the properties of the association.
    *
    * @param coder the decoder to use.
    * @param version the version to consider.
    * @param topicResolver the function to use for resolving topic references.
    */
   void decodeProperties(contomap::infrastructure::serial::Decoder &coder, uint8_t version,
      std::function<contomap::model::Topic &(contomap::model::Identifier)> const &topicResolver);

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
    * Moves the association by the given offset.
    *
    * @param offset the offset to apply.
    */
   void moveBy(SpacialCoordinate::Offset offset);

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

   void setAppearance(contomap::model::Style style) override;
   [[nodiscard]] contomap::model::Style getAppearance() const override;

private:
   class RoleEntry
   {
   public:
      explicit RoleEntry(std::unique_ptr<contomap::infrastructure::Link<contomap::model::Role>> link)
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
   contomap::model::Coordinates location;
   contomap::model::Style appearance;
   std::map<contomap::model::Identifier, std::unique_ptr<RoleEntry>> roles;
};

}
