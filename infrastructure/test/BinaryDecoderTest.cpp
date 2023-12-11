#include <gtest/gtest.h>

#include "contomap/infrastructure/serial/BinaryDecoder.h"

using contomap::infrastructure::serial::BinaryDecoder;
using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;

TEST(DecoderTest, codeChar)
{
   std::vector<uint8_t> data { 65 };
   BinaryDecoder decoder(data.data(), data.data() + data.size());
   char value = 0;
   decoder.code("", value);
   EXPECT_EQ('A', value);
}

TEST(DecoderTest, codeByte)
{
   std::vector<uint8_t> data { 0x20 };
   BinaryDecoder decoder(data.data(), data.data() + data.size());
   uint8_t value = 0;
   decoder.code("", value);
   EXPECT_EQ(0x20, value);
}

TEST(DecoderTest, codeFloatA)
{
   std::vector<uint8_t> data { 0x00, 0x00, 0x00, 0x00 };
   BinaryDecoder decoder(data.data(), data.data() + data.size());
   float value = 1234.5678f;
   decoder.code("", value);
   EXPECT_EQ(0, value);
}

TEST(DecoderTest, codeFloatB)
{
   std::vector<uint8_t> data { 0x3F, 0x80, 0x00, 0x00 };
   BinaryDecoder decoder(data.data(), data.data() + data.size());
   float value = 1234.5678f;
   decoder.code("", value);
   EXPECT_NEAR(1.0f, value, 0x00001f);
}

TEST(DecoderTest, codeFloatC)
{
   std::vector<uint8_t> data { 0x34, 0x00, 0x00, 0x00 };
   BinaryDecoder decoder(data.data(), data.data() + data.size());
   float value = 1234.5678f;
   decoder.code("", value);
   EXPECT_NEAR(std::numeric_limits<float>::epsilon(), value, std::numeric_limits<float>::epsilon());
}

TEST(DecoderTest, codeString)
{
   std::vector<uint8_t> data { 0x31, 0x32, 0x33, 0x34, 0x00 };
   BinaryDecoder decoder(data.data(), data.data() + data.size());
   std::string value;
   decoder.code("", value);
   EXPECT_EQ("1234", value);
}

TEST(DecoderTest, codeArray)
{
   std::vector<uint8_t> data { 0x00, 0x00, 0x03, 0x10, 0x20, 0x30 };
   BinaryDecoder decoder(data.data(), data.data() + data.size());
   std::array<uint8_t, 3> arr { 0x00, 0x00, 0x00 };
   decoder.codeArray("", [&arr](Decoder &nested, size_t index) {
      ASSERT_LT(index, 3);
      nested.code("", arr[index]);
   });
   std::array<uint8_t, 3> expected { 0x10, 0x20, 0x30 };
   EXPECT_EQ(expected, arr);
}

TEST(DecoderTest, codeScope)
{
   std::vector<uint8_t> data { 0xFF, 0x00, 0x00, 0x05, 0x31, 0x32, 0x33, 0x34, 0x00, 0xAA };
   BinaryDecoder decoder(data.data(), data.data() + data.size());

   uint8_t marker1 = 0x00;
   decoder.code("", marker1);
   EXPECT_EQ(0xFF, marker1);
   {
      Coder::Scope scope(decoder, "");
      std::string value;
      decoder.code("", value);
      EXPECT_EQ("1234", value);
   }
   uint8_t marker2 = 0x00;
   decoder.code("", marker2);
   EXPECT_EQ(0xAA, marker2);
}

TEST(DecoderTest, exceptionPastEnd)
{
   std::vector<uint8_t> data { 0x00 };
   BinaryDecoder decoder(data.data(), data.data() + data.size());
   float value = 0.0f;
   try
   {
      decoder.code("", value);
      FAIL() << "no exception";
   }
   catch (std::exception &)
   {
   }
}

TEST(DecoderTest, mismatchedScopesPutStreamToEnd)
{
   std::vector<uint8_t> data { 0x00, 0x00, 0x05, 0x00, 0x00, 0x01, 0x10, 0x11, 0x12 };
   BinaryDecoder decoder(data.data(), data.data() + data.size());
   auto scope1 = std::make_unique<Coder::Scope>(decoder, "");
   auto scope2 = std::make_unique<Coder::Scope>(decoder, "");
   scope1.reset();
   uint8_t value = 0x00;
   decoder.code("", value);
   scope2.reset();
   try
   {
      decoder.code("", value);
      FAIL() << "no exception";
   }
   catch (std::exception &)
   {
   }
}

TEST(DecoderTest, guardReadingPastEndFromWrongScopeLength)
{
   std::vector<uint8_t> data { 0x00, 0x00, 0x05 };
   BinaryDecoder decoder(data.data(), data.data() + data.size());
   auto scope = std::make_unique<Coder::Scope>(decoder, "");
   scope.reset();
   try
   {
      uint8_t value = 0xFF;
      decoder.code("", value);
      FAIL() << "no exception";
   }
   catch (std::exception &)
   {
   }
}
