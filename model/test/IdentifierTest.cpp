#include <sstream>

#include <gtest/gtest.h>

#include "contomap/model/Identifier.h"

using contomap::model::Identifier;

TEST(IdentifierTest, fromFailures)
{
   EXPECT_EQ(0, Identifier::from({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }).index()) << "wrong characters allowed";
}

TEST(IdentifierTest, comparison)
{
   auto a = std::get<Identifier>(Identifier::from({ 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a' }));
   auto b = std::get<Identifier>(Identifier::from({ 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b' }));
   auto c = std::get<Identifier>(Identifier::from({ 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c' }));

   EXPECT_EQ(a, a);
   EXPECT_EQ(b, b);
   EXPECT_EQ(c, c);

   EXPECT_NE(a, b);
   EXPECT_NE(b, c);
   EXPECT_NE(a, c);

   EXPECT_LT(a, b);
   EXPECT_LT(b, c);
   EXPECT_LT(a, c);

   EXPECT_GT(b, a);
   EXPECT_GT(c, b);
   EXPECT_GT(c, a);
}

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
   Identifier id = std::get<Identifier>(Identifier::from({ 'a', '0', 't', 'e', 's', 't', 'Z', 'z', '0', '1', '2', '3' }));
   std::ostringstream buf;
   buf << id;
   EXPECT_EQ("a0testZz0123", buf.str());
}
