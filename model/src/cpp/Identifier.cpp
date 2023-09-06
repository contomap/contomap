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
