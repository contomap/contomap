#pragma once

#include <functional>
#include <optional>

#include "contomap/model/Identifier.h"

namespace contomap::model
{

/**
 * OptionalIdentifier is a single-cell container for an Identifier value.
 * The container can be set, or can be empty.
 */
class OptionalIdentifier
{
public:
   /**
    * Default constructor for an unassigned container.
    */
   OptionalIdentifier() = default;

   /**
    * Create an initialized container with given identifier.
    *
    * @param id the identifier to capture.
    * @return an initialized container.
    */
   [[nodiscard]] static OptionalIdentifier of(Identifier id);

   /**
    * @return true if an identifier is specified for this container, false otherwise.
    */
   [[nodiscard]] bool isAssigned() const;

   /**
    * Remove the assignment of an identifier.
    */
   void clear();

   /**
    * Write the given identifier to the given stream.
    *
    * @param stream the stream to write to
    * @param identifier the identifier to write
    * @return the provided stream
    */
   friend std::ostream &operator<<(std::ostream &stream, OptionalIdentifier const &identifier)
   {
      if (identifier.isAssigned())
      {
         stream << identifier.value.value();
      }
      else
      {
         stream << std::string(Identifier::ValueType {}.size(), '-');
      }
      return stream;
   }

   /**
    * Spaceship operator.
    *
    * @param other the other instance to compare to.
    * @return the ordering for this type.
    */
   std::strong_ordering operator<=>(OptionalIdentifier const &other) const noexcept = default;

   /**
    * Assigns given identifier to this container.
    *
    * @param id the identifier to assign.
    * @return this.
    */
   OptionalIdentifier &operator=(Identifier id);

private:
   explicit OptionalIdentifier(Identifier value);

   std::optional<Identifier> value;
};

}
