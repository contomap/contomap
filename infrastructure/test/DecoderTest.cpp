#include <gtest/gtest.h>

#include "contomap/infrastructure/serial/Decoder.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;

TEST(DecoderTest, codeChar)
{
   std::vector<uint8_t> data { 65 };
   Decoder decoder(data.data(), data.data() + data.size());
   Coder &coder = decoder;
   char value = 0;
   coder.code("", value);
   EXPECT_EQ('A', value);
}

TEST(DecoderTest, codeByte)
{
   std::vector<uint8_t> data { 0x20 };
   Decoder decoder(data.data(), data.data() + data.size());
   Coder &coder = decoder;
   uint8_t value = 0;
   coder.code("", value);
   EXPECT_EQ(0x20, value);
}

TEST(DecoderTest, codeFloatA)
{
   std::vector<uint8_t> data { 0x00, 0x00, 0x00, 0x00 };
   Decoder decoder(data.data(), data.data() + data.size());
   Coder &coder = decoder;
   float value = 1234.5678f;
   coder.code("", value);
   EXPECT_EQ(0, value);
}

TEST(DecoderTest, codeFloatB)
{
   std::vector<uint8_t> data { 0x3F, 0x80, 0x00, 0x00 };
   Decoder decoder(data.data(), data.data() + data.size());
   Coder &coder = decoder;
   float value = 1234.5678f;
   coder.code("", value);
   EXPECT_NEAR(1.0f, value, 0x00001f);
}

TEST(DecoderTest, codeFloatC)
{
   std::vector<uint8_t> data { 0x34, 0x00, 0x00, 0x00 };
   Decoder decoder(data.data(), data.data() + data.size());
   Coder &coder = decoder;
   float value = 1234.5678f;
   coder.code("", value);
   EXPECT_NEAR(std::numeric_limits<float>::epsilon(), value, std::numeric_limits<float>::epsilon());
}

TEST(DecoderTest, codeString)
{
   std::vector<uint8_t> data { 0x31, 0x32, 0x33, 0x34, 0x00 };
   Decoder decoder(data.data(), data.data() + data.size());
   Coder &coder = decoder;
   std::string value;
   coder.code("", value);
   EXPECT_EQ("1234", value);
}

TEST(DecoderTest, codeArray)
{
   std::vector<uint8_t> data { 0x00, 0x00, 0x03, 0x10, 0x20, 0x30 };
   Decoder decoder(data.data(), data.data() + data.size());
   Coder &coder = decoder;
   std::array<uint8_t, 3> arr { 0x00, 0x00, 0x00 };
   coder.code("", arr.size(), [&arr](size_t index, Coder &c) {
      ASSERT_LT(index, 3);
      c.code("", arr[index]);
   });
   std::array<uint8_t, 3> expected { 0x10, 0x20, 0x30 };
   EXPECT_EQ(expected, arr);
}

TEST(DecoderTest, codeScope)
{
   std::vector<uint8_t> data { 0xFF, 0x00, 0x00, 0x05, 0x31, 0x32, 0x33, 0x34, 0x00, 0xAA };
   Decoder decoder(data.data(), data.data() + data.size());
   Coder &coder = decoder;

   uint8_t marker1 = 0x00;
   coder.code("", marker1);
   EXPECT_EQ(0xFF, marker1);
   {
      Coder::Scope scope(coder, "");
      std::string value;
      coder.code("", value);
      EXPECT_EQ("1234", value);
   }
   uint8_t marker2 = 0x00;
   coder.code("", marker2);
   EXPECT_EQ(0xAA, marker2);
}

// TODO: tests for wrong offsets
// - exception when reading past the end
// - guarding scope end to be outside allowed range