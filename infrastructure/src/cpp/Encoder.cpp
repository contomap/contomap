#include <bit>

#include "contomap/infrastructure/serial/Encoder.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Encoder;

std::vector<uint8_t> const &Encoder::getData() const
{
   return data;
}

void Encoder::code(std::string const &name, uint8_t &value)
{
   data.push_back(value);
}

void Encoder::code(std::string const &name, float &value)
{
   auto raw = reinterpret_cast<uint8_t *>(&value);

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

void Encoder::code(std::string const &name, std::string &value)
{
   for (char c : value)
   {
      data.push_back(static_cast<uint8_t>(c));
   }
   data.push_back(0x00);
}

uintptr_t Encoder::codeScopeBegin(std::string const &name)
{
   data.push_back(0x00);
   data.push_back(0x00);
   data.push_back(0x00);
   return static_cast<uintptr_t>(data.size()) - 3;
}

void Encoder::codeScopeEnd(uintptr_t tag)
{
   size_t size = data.size() - tag - 3;
   data[tag + 0] = static_cast<uint8_t>(size >> 16);
   data[tag + 1] = static_cast<uint8_t>(size >> 8);
   data[tag + 2] = static_cast<uint8_t>(size >> 0);
}

uintptr_t Encoder::codeArrayBegin(std::string const &name, size_t &size)
{
   data.push_back(static_cast<uint8_t>(size >> 16));
   data.push_back(static_cast<uint8_t>(size >> 8));
   data.push_back(static_cast<uint8_t>(size >> 0));
   return 0;
}

void Encoder::codeArrayEnd(uintptr_t)
{
}
