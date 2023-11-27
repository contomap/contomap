#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace contomap::infrastructure::serial
{

/**
 * Coder is a general interface for symmetrical data serialization.
 */
class Coder
{
public:
   /**
    * Scope represents a new, named structure.
    */
   class Scope
   {
   public:
      /**
       * Constructor.
       *
       * @param coder the coder reference to work on
       * @param name the name of the scope.
       */
      Scope(Coder &coder, std::string const &name)
         : coder(coder)
         , tag(coder.codeScopeBegin(name))
      {
      }
      ~Scope()
      {
         coder.codeScopeEnd(tag);
      }

   private:
      Coder &coder;
      uintptr_t tag;
   };

   virtual ~Coder() = default;

   /**
    * Serialize a single character.
    *
    * @param name the name of the value.
    * @param value the value.
    */
   void code(std::string const &name, char &value)
   {
      code(name, reinterpret_cast<uint8_t &>(value));
   }
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
    * @param count the current number of entries.
    * @param deferrer called to serialize an element of the array.
    */
   void code(std::string const &name, size_t count, std::function<void(size_t, Coder &)> const &deferrer)
   {
      size_t codedCount = count;
      uintptr_t tag = codeArrayBegin(name, codedCount);
      for (size_t index = 0; index < codedCount; index++)
      {
         deferrer(index, *this);
      }
      codeArrayEnd(tag);
   }

protected:
   /**
    * Serialize the begin of a new scope.
    * This function will be called by Scope.
    *
    * @param name the name of the structure.
    * @return a reference value, specific to the coder, that is provided back to the end function.
    */
   virtual uintptr_t codeScopeBegin(std::string const &name) = 0;
   /**
    * Finishes the previously created scope.
    * This function will be called by Scope.
    *
    * @param tag the returned value for the respective begin call.
    */
   virtual void codeScopeEnd(uintptr_t tag) = 0;

   /**
    * Serialize the begin of a new array.
    * This function will be called by coding an array.
    *
    * @param name the name of the array.
    * @param size the number of elements in the array.
    * @return a reference value, specific to the coder, that is provided back to the end function.
    */
   virtual uintptr_t codeArrayBegin(std::string const &name, size_t &size) = 0;
   /**
    * Serialize the end of the previously created array.
    *
    * @param tag the returned value for the respective begin call.
    */
   virtual void codeArrayEnd(uintptr_t tag) = 0;

private:
   friend Scope;
};
}
