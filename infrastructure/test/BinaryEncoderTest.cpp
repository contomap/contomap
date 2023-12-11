#include <gtest/gtest.h>

#include "contomap/infrastructure/serial/BinaryEncoder.h"

using contomap::infrastructure::serial::BinaryEncoder;
using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Encoder;

static void expectEncoded(BinaryEncoder &coder, std::vector<uint8_t> const &expected)
{
   EXPECT_EQ(expected, coder.getData());
}

TEST(EncoderTest, codeChar)
{
   BinaryEncoder encoder;
   char value = 'A';
   encoder.code("", value);
   expectEncoded(encoder, { 65 });
}

TEST(EncoderTest, codeByte)
{
   BinaryEncoder encoder;
   uint8_t value = 0xCD;
   encoder.code("", value);
   expectEncoded(encoder, { 0xCD });
}

TEST(EncoderTest, codeFloatA)
{
   BinaryEncoder encoder;
   float value = 0.0f;
   encoder.code("", value);
   expectEncoded(encoder, { 0x00, 0x00, 0x00, 0x00 });
}

TEST(EncoderTest, codeFloatB)
{
   BinaryEncoder encoder;

   float value = 1.0f;
   encoder.code("", value);
   expectEncoded(encoder, { 0x3F, 0x80, 0x00, 0x00 });
}

TEST(EncoderTest, codeFloatC)
{
   BinaryEncoder encoder;
   float value = std::numeric_limits<float>::epsilon();
   encoder.code("", value);
   expectEncoded(encoder, { 0x34, 0x00, 0x00, 0x00 });
}

TEST(EncoderTest, codeString)
{
   BinaryEncoder encoder;
   std::string value("1234");
   encoder.code("", value);
   expectEncoded(encoder, { 0x31, 0x32, 0x33, 0x34, 0x00 });
}

TEST(EncoderTest, codeArray)
{
   BinaryEncoder encoder;
   std::array<uint8_t, 3> arr { 0x10, 0x20, 0x30 };
   encoder.codeArray("", arr.begin(), arr.end(), [](Encoder &nested, char const &c) { nested.code("", c); });
   expectEncoded(encoder, { 0x00, 0x00, 0x03, 0x10, 0x20, 0x30 });
}

TEST(EncoderTest, codeScope)
{
   BinaryEncoder encoder;
   uint8_t marker = 0xFF;
   encoder.code("", marker);
   {
      Coder::Scope scope(encoder, "");
      std::string value("1234");
      encoder.code("", value);
   }
   encoder.code("", marker);
   expectEncoded(encoder, { 0xFF, 0x00, 0x00, 0x05, 0x31, 0x32, 0x33, 0x34, 0x00, 0xFF });
}
