#pragma once

#include "contomap/infrastructure/serial/Coder.h"

namespace contomap::infrastructure::serial
{

/**
 * Decoder is used to deserialize data.
 */
class Decoder : public contomap::infrastructure::serial::Coder
{
public:
   ~Decoder() override = default;

   /**
    * Serialize a single character.
    *
    * @param name the name of the value.
    * @param value the value.
    */
   virtual void code(std::string const &name, char &value) = 0;

   /**
    * Serialize a single byte.
    *
    * @param name the name of the value.
    * @param value the value.
    */
   virtual void code(std::string const &name, uint8_t &value) = 0;
   /**
    * Serialize a floating point value.
    *
    * @param name the name of the value.
    * @param value the value.
    */
   virtual void code(std::string const &name, float &value) = 0;
   /**
    * Serialize a string value.
    *
    * @param name the name of the value.
    * @param value the value.
    */
   virtual void code(std::string const &name, std::string &value) = 0;

   /**
    * Serialize an array.
    * The deferrer must be able to cope with indices beyond its current limit. This represents the decoding case, during which
    * the function needs to create a necessary instance, and then serialize this value.
    *
    * @param name the name of the array.
    * @param deferrer called to serialize an element of the array.
    */
   void codeArray(std::string const &name, std::function<void(Decoder &, size_t)> const &deferrer)
   {
      size_t codedCount = 0;
      uintptr_t tag = codeArrayBegin(name, codedCount);
      for (size_t index = 0; index < codedCount; index++)
      {
         deferrer(*this, index);
      }
      codeArrayEnd(tag);
   }

protected:
   /**
    * Serialize the begin of a new array.
    * This function will be called by coding an array.
    *
    * @param name the name of the array.
    * @param size the number of elements in the array.
    * @return a reference value, specific to the coder, that is provided back to the end function.
    */
   [[nodiscard]] virtual uintptr_t codeArrayBegin(std::string const &name, size_t &size) = 0;
   /**
    * Serialize the end of the previously created array.
    *
    * @param tag the returned value for the respective begin call.
    */
   virtual void codeArrayEnd(uintptr_t tag) = 0;
};
}
