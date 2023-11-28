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

protected:
   /**
    * Serialize the begin of a new scope.
    * This function will be called by Scope.
    *
    * @param name the name of the structure.
    * @return a reference value, specific to the coder, that is provided back to the end function.
    */
   [[nodiscard]] virtual uintptr_t codeScopeBegin(std::string const &name) = 0;
   /**
    * Finishes the previously created scope.
    * This function will be called by Scope.
    *
    * @param tag the returned value for the respective begin call.
    */
   virtual void codeScopeEnd(uintptr_t tag) = 0;

private:
   friend Scope;
};
}
