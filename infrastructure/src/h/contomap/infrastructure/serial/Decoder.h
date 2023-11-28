#pragma once

#include <vector>

#include "contomap/infrastructure/serial/Coder.h"

namespace contomap::infrastructure::serial
{

/**
 * Decoder decodes data from a byte range.
 */
class Decoder : public contomap::infrastructure::serial::Coder
{
public:
   /**
    * Constructor.
    *
    * @param begin the first byte to read from.
    * @param end the end marker offset, the first byte after the valid range.
    */
   Decoder(uint8_t const *begin, uint8_t const *end);

   void code(std::string const &name, uint8_t &value) override;
   void code(std::string const &name, float &value) override;
   void code(std::string const &name, std::string &value) override;

protected:
   uintptr_t codeScopeBegin(std::string const &name) override;
   void codeScopeEnd(uintptr_t tag) override;

   uintptr_t codeArrayBegin(std::string const &name, size_t &size) override;
   void codeArrayEnd(uintptr_t tag) override;

private:
   size_t readSize();
   uint8_t nextByte();

   uint8_t const *end;
   uint8_t const *current;
};

}
