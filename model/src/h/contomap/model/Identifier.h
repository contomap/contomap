#pragma once

#include <array>
#include <ostream>
#include <string>
#include <variant>

namespace contomap::model
{

/**
 * Identifier is a value to be used as a unique key for model entries.
 * Internally, it is based on 8 characters from a defined set of possible values.
 */
class Identifier
{
public:
   /** The internally used type. */
   using ValueType = std::array<char, 8>;

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

   constexpr std::strong_ordering operator<=>(Identifier const &other) const noexcept = default;

private:
   explicit Identifier(ValueType const &value);

   static std::string const ALLOWED_CHARACTERS;

   ValueType value;
};

}
