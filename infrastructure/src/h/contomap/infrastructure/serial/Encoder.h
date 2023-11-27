#pragma once

#include <vector>

#include "contomap/infrastructure/serial/Coder.h"

namespace contomap::infrastructure::serial
{

/**
 * Encoder encodes the values into a simple byte array.
 * It is stored in big-endian notation, and strings are encoded with variable length and a 0x00 terminator.
 */
class Encoder : public contomap::infrastructure::serial::Coder
{
public:
   /**
    * @return the encoded data.
    */
   [[nodiscard]] std::vector<uint8_t> const &getData() const;

   void code(std::string const &name, uint8_t &value) override;
   void code(std::string const &name, float &value) override;
   void code(std::string const &name, std::string &value) override;

protected:
   uintptr_t codeScopeBegin(std::string const &name) override;
   void codeScopeEnd(uintptr_t tag) override;

   uintptr_t codeArrayBegin(std::string const &name, size_t &size) override;
   void codeArrayEnd(uintptr_t tag) override;

private:
   std::vector<uint8_t> data;
};

}
