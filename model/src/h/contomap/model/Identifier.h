#pragma once

#include <array>
#include <ostream>
#include <string>
#include <variant>

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
    * Creates a new Identifier from the given raw value.
    *
    * TODO replace return value with a better type
    *
    * @param value the raw value to take over
    * @return either the right type of the created identifier, or an error instead.
    */
   [[nodiscard]] static std::variant<std::monostate, Identifier> from(ValueType value);

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

private:
   explicit Identifier(ValueType const &value);

   static std::string const ALLOWED_CHARACTERS;

   ValueType value;
};

}
