#pragma once

#include <array>
#include <ostream>
#include <string>

#include "contomap/infrastructure/serial/Coder.h"

namespace contomap::model
{

/**
 * Identifier is a value to be used as a unique key for model entries.
 * Internally, it is based on 12 characters from a defined set of possible values.
 */
class Identifier
{
public:
   /** The internally used type. */
   using ValueType = std::array<char, 12>;

   /**
    * Create an identifier from a saved state.
    *
    * @param name the name to use for the field.
    * @param coder the coder to use.
    * @return the deserialized instance.
    */
   [[nodiscard]] static Identifier from(std::string const &name, contomap::infrastructure::serial::Coder &coder);

   /**
    * Create a random identifier.
    *
    * According to Nano ID, a 1% chance of a duplicate would come after around 2-8 billion IDs.
    * The range is 2-8 billion IDs is because the internally used set allows for letters and
    * digits, however the algorithm tries to avoid creating potential words.
    *
    * \see https://zelark.github.io/nano-id-cc/
    *
    * @return a new random identifier
    */
   [[nodiscard]] static Identifier random();

   /**
    * Write the given identifier to the given stream.
    *
    * @param stream the stream to write to
    * @param identifier the identifier to write
    * @return the provided stream
    */
   friend std::ostream &operator<<(std::ostream &stream, Identifier const &identifier)
   {
      for (char part : identifier.value)
      {
         stream << part;
      }
      return stream;
   }

   /**
    * Spaceship operator.
    *
    * @param other the other instance to compare to.
    * @return the ordering for this type.
    */
   std::strong_ordering operator<=>(Identifier const &other) const noexcept = default;

   /**
    * Serializes this identifier with given coder.
    *
    * @param name the name to use for the field.
    * @param coder the coder to use.
    */
   void code(std::string const &name, contomap::infrastructure::serial::Coder &coder);

private:
   explicit Identifier(ValueType const &value);

   static void code(std::string const &name, contomap::infrastructure::serial::Coder &coder, ValueType &value);

   static std::string const ALLOWED_CHARACTERS;

   ValueType value;
};

}
