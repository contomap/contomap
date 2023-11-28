#include <bit>

#include "contomap/infrastructure/serial/BinaryEncoder.h"

using contomap::infrastructure::serial::BinaryEncoder;

std::vector<uint8_t> const &BinaryEncoder::getData() const
{
   return data;
}

void BinaryEncoder::code(std::string const &name, char const &value)
{
   data.push_back(static_cast<uint8_t>(value));
}

void BinaryEncoder::code(std::string const &name, uint8_t const &value)
{
   data.push_back(value);
}

void BinaryEncoder::code(std::string const &name, float const &value)
{
   auto raw = reinterpret_cast<uint8_t const *>(&value);

   if constexpr (std::endian::native == std::endian::big)
   {
      data.push_back(raw[0]);
      data.push_back(raw[1]);
      data.push_back(raw[2]);
      data.push_back(raw[3]);
   }
   else
   {
      data.push_back(raw[3]);
      data.push_back(raw[2]);
      data.push_back(raw[1]);
      data.push_back(raw[0]);
   }
}

void BinaryEncoder::code(std::string const &name, std::string const &value)
{
   for (char c : value)
   {
      data.push_back(static_cast<uint8_t>(c));
   }
   data.push_back(0x00);
}

uintptr_t BinaryEncoder::codeScopeBegin(std::string const &name)
{
   data.push_back(0x00);
   data.push_back(0x00);
   data.push_back(0x00);
   return static_cast<uintptr_t>(data.size()) - 3;
}

void BinaryEncoder::codeScopeEnd(uintptr_t tag)
{
   size_t size = data.size() - tag - 3;
   data[tag + 0] = static_cast<uint8_t>(size >> 16);
   data[tag + 1] = static_cast<uint8_t>(size >> 8);
   data[tag + 2] = static_cast<uint8_t>(size >> 0);
}

uintptr_t BinaryEncoder::codeArrayBegin(std::string const &name)
{
   data.push_back(0x00);
   data.push_back(0x00);
   data.push_back(0x00);
   return static_cast<uintptr_t>(data.size()) - 3;
}

void BinaryEncoder::codeArrayEnd(uintptr_t tag, size_t size)
{
   data[tag + 0] = static_cast<uint8_t>(size >> 16);
   data[tag + 1] = static_cast<uint8_t>(size >> 8);
   data[tag + 2] = static_cast<uint8_t>(size >> 0);
}
