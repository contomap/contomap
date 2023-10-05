#pragma once

#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/OptionalIdentifier.h"

namespace contomap::model
{

class Association;

/**
 * A Role represents the nature a topic plays in an association.
 */
class Role
{
public:
   /**
    * Seed contains the basic information for creating a new role.
    * Seed instances are created through Association objects.
    */
   class Seed
   {
   public:
      Seed() = delete;
      /**
       * Default copy constructor.
       */
      Seed(Seed const &) = default;
      /**
       * Default move constructor.
       */
      Seed(Seed &&) = default;

      /**
       * Default assignment operator.
       * @return default result.
       */
      Seed &operator=(Seed const &) = default;
      /**
       * Default move operator.
       * @return default result.
       */
      Seed &operator=(Seed &&) = default;

      /**
       * @return the unique identifier for the role.
       */
      [[nodiscard]] contomap::model::Identifier getId() const
      {
         return id;
      }

   private:
      Seed(contomap::model::Identifier id, contomap::model::Identifier parent)
         : id(id)
         , parent(parent)
      {
      }

      friend Role;
      friend Association;

      contomap::model::Identifier id;
      contomap::model::Identifier parent;
   };

   /**
    * Constructor.
    *
    * @param seed the properties for creating a new Role. Retrieve a new instance from an Association.
    */
   explicit Role(Seed const &seed);

   /**
    * @return the primary identifier of this role.
    */
   [[nodiscard]] Identifier getId() const;

   /**
    * @return the primary identifier of the association this role is part of.
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
