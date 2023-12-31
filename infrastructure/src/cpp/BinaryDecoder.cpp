#include <bit>
#include <sstream>

#include "contomap/infrastructure/serial/BinaryDecoder.h"

using contomap::infrastructure::serial::BinaryDecoder;

BinaryDecoder::BinaryDecoder(uint8_t const *begin, uint8_t const *end)
   : end(end)
   , current(begin)
{
}

void BinaryDecoder::code(std::string const &name, char &value)
{
   value = static_cast<char>(nextByte());
}

void BinaryDecoder::code(std::string const &name, uint8_t &value)
{
   value = nextByte();
}

void BinaryDecoder::code(std::string const &name, float &value)
{
   auto raw = reinterpret_cast<uint8_t *>(&value);

   if constexpr (std::endian::native == std::endian::big)
   {
      raw[0] = nextByte();
      raw[1] = nextByte();
      raw[2] = nextByte();
      raw[3] = nextByte();
   }
   else
   {
      raw[3] = nextByte();
      raw[2] = nextByte();
      raw[1] = nextByte();
      raw[0] = nextByte();
   }
}

void BinaryDecoder::code(std::string const &name, std::string &value)
{
   std::ostringstream buf;
   for (uint8_t b = nextByte(); b != 0x00; b = nextByte())
   {
      buf << static_cast<char>(b);
   }
   value = buf.str();
}

uintptr_t BinaryDecoder::codeScopeBegin(std::string const &name)
{
   size_t offset = readSize();
   return reinterpret_cast<uintptr_t>(current + offset);
}

void BinaryDecoder::codeScopeEnd(uintptr_t tag)
{
   auto newEnd = reinterpret_cast<uint8_t const *>(tag);
   current = ((newEnd >= current) && (newEnd <= end)) ? newEnd : end;
}

uintptr_t BinaryDecoder::codeArrayBegin(std::string const &name, size_t &size)
{
   size = readSize();
   return 0;
}

void BinaryDecoder::codeArrayEnd(uintptr_t)
{
}

size_t BinaryDecoder::readSize()
{
   size_t size = nextByte();
   size <<= 8;
   size += nextByte();
   size <<= 8;
   size += nextByte();
   return size;
}

uint8_t BinaryDecoder::nextByte()
{
   if (current >= end)
   {
      throw std::runtime_error("reading past end");
   }
   return *current++;
}
