#pragma once

#include <functional>
#include <optional>

#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
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
    * Deserialize an optional identifier.
    *
    * @param coder the decoder to use.
    * @param name the name to use for the scope.
    * @return the decoded instance
    */
   [[nodiscard]] static OptionalIdentifier from(contomap::infrastructure::serial::Decoder &coder, std::string const &name);

   /**
    * Serialize the optional identifier value.
    *
    * @param coder the encoder to use.
    * @param name the name to use for the scope.
    */
   void encode(contomap::infrastructure::serial::Encoder &coder, std::string const &name) const;

   /**
    * @return true if an identifier is specified for this container, false otherwise.
    */
   [[nodiscard]] bool isAssigned() const;

   /**
    * @return the contained value - if it is assigned.
    */
   [[nodiscard]] contomap::model::Identifier value() const;

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
         stream << identifier.content.value();
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

   std::optional<Identifier> content;
};

}
