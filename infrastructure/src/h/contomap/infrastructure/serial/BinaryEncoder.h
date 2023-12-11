#pragma once

#include <vector>

#include "contomap/infrastructure/serial/Encoder.h"

namespace contomap::infrastructure::serial
{

/**
 * BinaryEncoder encodes the values into a simple byte array.
 * It is stored in big-endian notation, and strings are encoded with variable length and a 0x00 terminator.
 */
class BinaryEncoder : public contomap::infrastructure::serial::Encoder
{
public:
   /**
    * @return the encoded data.
    */
   [[nodiscard]] std::vector<uint8_t> const &getData() const;

   void code(std::string const &name, char const &value) override;
   void code(std::string const &name, uint8_t const &value) override;
   void code(std::string const &name, float const &value) override;
   void code(std::string const &name, std::string const &value) override;

protected:
   [[nodiscard]] uintptr_t codeScopeBegin(std::string const &name) override;
   void codeScopeEnd(uintptr_t tag) override;

   [[nodiscard]] uintptr_t codeArrayBegin(std::string const &name) override;
   void codeArrayEnd(uintptr_t tag, size_t size) override;

private:
   std::vector<uint8_t> data;
};

}
