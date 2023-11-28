#pragma once

#include "contomap/infrastructure/serial/Coder.h"

namespace contomap::infrastructure::serial
{

/**
 * Encoder is used to serialize data.
 */
class Encoder : public contomap::infrastructure::serial::Coder
{
public:
   ~Encoder() override = default;

   /**
    * Serialize a single character.
    *
    * @param name the name of the value.
    * @param value the value.
    */
   virtual void code(std::string const &name, char const &value) = 0;
   /**
    * Serialize a single byte.
    *
    * @param name the name of the value.
    * @param value the value.
    */
   virtual void code(std::string const &name, uint8_t const &value) = 0;
   /**
    * Serialize a floating point value.
    *
    * @param name the name of the value.
    * @param value the value.
    */
   virtual void code(std::string const &name, float const &value) = 0;
   /**
    * Serialize a string value.
    *
    * @param name the name of the value.
    * @param value the value.
    */
   virtual void code(std::string const &name, std::string const &value) = 0;

   /**
    * Serialize an array.
    * The deferrer will be called with the reference of each iterator
    *
    * @param name the name of the array.
    * @param first the first entry of the range.
    * @param last the end marker of the range.
    * @param deferrer called to serialize an element of the array.
    */
   template <class InputIt, class Function> void codeArray(std::string const &name, InputIt first, InputIt last, Function const &deferrer)
   {
      uintptr_t tag = codeArrayBegin(name);
      size_t size = 0;
      while (first != last)
      {
         deferrer(*this, *first);
         ++first;
         ++size;
      }
      codeArrayEnd(tag, size);
   }

protected:
   /**
    * Serialize the begin of a new array.
    * This function will be called by coding an array.
    *
    * @param name the name of the array.
    * @return a reference value, specific to the coder, that is provided back to the end function.
    */
   [[nodiscard]] virtual uintptr_t codeArrayBegin(std::string const &name) = 0;
   /**
    * Serialize the end of the previously created array.
    *
    * @param size the number of elements in the array.
    * @param tag the returned value for the respective begin call.
    */
   virtual void codeArrayEnd(uintptr_t tag, size_t size) = 0;
};
}
