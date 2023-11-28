#include <regex>
#include <sstream>

#include <gtest/gtest.h>

#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/Identifier.h"

using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Identifier;

TEST(IdentifierTest, randomIdentifierAreUnique)
{
   std::set<Identifier> created;
   size_t attempts = 1000;
   for (size_t i = 0; i < attempts; i++)
   {
      created.emplace(Identifier::random());
   }
   EXPECT_EQ(attempts, created.size());
}

TEST(IdentifierTest, shiftToOutputStream)
{
   std::regex pattern("^[a-zA-Z0-9]{12}$");
   auto id = Identifier::random();
   std::ostringstream buf;
   buf << id;
   EXPECT_TRUE(std::regex_match(buf.str(), pattern)) << "not matched: '" << id << "'";
}

TEST(IdentifierTest, serialization)
{
   auto id = Identifier::random();
   Encoder encoder;
   id.code("", encoder);
   auto data = encoder.getData();
   Decoder decoder(data.data(), data.data() + data.size());
   auto copy = Identifier::from("", decoder);
   EXPECT_EQ(id, copy);
}
