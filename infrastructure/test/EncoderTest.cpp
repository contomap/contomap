#include <gtest/gtest.h>

#include "contomap/infrastructure/serial/Encoder.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Encoder;

static void expectEncoded(Encoder &coder, std::vector<uint8_t> const &expected)
{
   EXPECT_EQ(expected, coder.getData());
}

TEST(EncoderTest, codeChar)
{
   Encoder encoder;
   Coder &coder = encoder;
   char value = 'A';
   coder.code("", value);
   expectEncoded(encoder, { 65 });
}

TEST(EncoderTest, codeByte)
{
   Encoder coder;

   uint8_t value = 0xCD;
   coder.code("", value);
   expectEncoded(coder, { 0xCD });
}

TEST(EncoderTest, codeFloatA)
{
   Encoder coder;

   float value = 0.0f;
   coder.code("", value);
   expectEncoded(coder, { 0x00, 0x00, 0x00, 0x00 });
}

TEST(EncoderTest, codeFloatB)
{
   Encoder coder;

   float value = 1.0f;
   coder.code("", value);
   expectEncoded(coder, { 0x3F, 0x80, 0x00, 0x00 });
}

TEST(EncoderTest, codeFloatC)
{
   Encoder coder;

   float value = std::numeric_limits<float>::epsilon();
   coder.code("", value);
   expectEncoded(coder, { 0x34, 0x00, 0x00, 0x00 });
}

TEST(EncoderTest, codeString)
{
   Encoder coder;

   std::string value("1234");
   coder.code("", value);
   expectEncoded(coder, { 0x31, 0x32, 0x33, 0x34, 0x00 });
}

TEST(EncoderTest, codeArray)
{
   Encoder encoder;

   std::array<uint8_t, 3> arr { 0x10, 0x20, 0x30 };
   Coder &coder = encoder;
   coder.code("", arr.size(), [&arr](size_t index, Coder &c) { c.code("", arr[index]); });
   expectEncoded(encoder, { 0x00, 0x00, 0x03, 0x10, 0x20, 0x30 });
}

TEST(EncoderTest, codeScope)
{
   Encoder encoder;
   Coder &coder = encoder;
   uint8_t marker = 0xFF;
   coder.code("", marker);
   {
      Coder::Scope scope(coder, "");
      std::string value("1234");
      coder.code("", value);
   }
   coder.code("", marker);
   expectEncoded(encoder, { 0xFF, 0x00, 0x00, 0x05, 0x31, 0x32, 0x33, 0x34, 0x00, 0xFF });
}
