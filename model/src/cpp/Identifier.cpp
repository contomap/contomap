#include <algorithm>
#include <random>

#include "contomap/model/Identifier.h"

using contomap::model::Identifier;

std::string const Identifier::ALLOWED_CHARACTERS("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

Identifier::Identifier(ValueType const &value)
   : value(value)
{
}

std::variant<std::monostate, Identifier> Identifier::from(ValueType value)
{
   if (std::find_first_of(value.begin(), value.end(), ALLOWED_CHARACTERS.begin(), ALLOWED_CHARACTERS.end()) == value.end())
   {
      return {};
   }
   return Identifier(value);
}

Identifier Identifier::random()
{
   // This algorithm creates a random identifier using the set of allowed characters, with the
   // extra requirement that no more than two letters are allowed in sequence. This is done
   // in order to avoid the accidental creation of words. There is still the potential that
   // words are created in "leet speak" by interpreting digits as letters, or that valid two-letter
   // words come out, yet this is negligible.
   static thread_local std::mt19937 gen([]() {
      std::random_device rd;
      return rd();
   }());
   std::uniform_int_distribution<size_t> allowedDigits(0, 9);
   std::uniform_int_distribution<size_t> allowedCharacters(0, ALLOWED_CHARACTERS.size() - 1);
   std::uniform_int_distribution<size_t> *currentSet = &allowedCharacters;
   ValueType value;
   size_t lettersInRow = 0;
   for (char &part : value)
   {
      size_t index = (*currentSet)(gen);
      part = ALLOWED_CHARACTERS[index];
      lettersInRow = (index < 10) ? 0 : (lettersInRow + 1);
      currentSet = (lettersInRow >= 2) ? &allowedDigits : &allowedCharacters;
   }
   return Identifier(value);
}
