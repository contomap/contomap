#pragma once

#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/OptionalIdentifier.h"

namespace contomap::model
{

/**
 * A Role represents the nature a topic plays in an association.
 */
class Role
{
public:
   /**
    * Constructor.
    *
    * @param id the primary identifier of this role.
    * @param parent the primary identifier of the association this role is part of.
    */
   Role(contomap::model::Identifier id, contomap::model::Identifier parent);

   /**
    * @return the primary identifier of this role.
    */
   [[nodiscard]] Identifier getId() const;

   /**
    * @return the primary identifier of the assocation this role is part of.
    */
   [[nodiscard]] Identifier getParent() const;

private:
   contomap::model::Identifier id;
   contomap::model::Identifier parent;

   contomap::model::OptionalIdentifier reifier;
   contomap::model::OptionalIdentifier type;

   contomap::model::OptionalIdentifier appearance;
};

}
