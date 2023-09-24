#include <gtest/gtest.h>

#include "contomap/model/Identifiers.h"

using contomap::model::Identifier;
using contomap::model::Identifiers;

TEST(IdentifiersTest, canBeIterated)
{
   Identifiers ids;
   ids.add(Identifier::random());
   ids.add(Identifier::random());
   ids.add(Identifier::random());
   std::set<Identifier> extractedIds;
   for (auto const &id : ids)
   {
      extractedIds.insert(id);
   }
   EXPECT_EQ(3, extractedIds.size());
}

TEST(IdentifiersTest, addAndRemove)
{
   Identifiers empty;
   Identifiers one;
   auto id = Identifier::random();
   one.add(id);
   EXPECT_NE(one, empty);
   EXPECT_FALSE(one.empty());
   one.remove(id);
   EXPECT_EQ(one, empty);
   EXPECT_TRUE(one.empty());
}

TEST(IdentifiersTest, clearing)
{
   Identifiers empty;
   Identifiers ids;
   ids.add(Identifier::random());
   ids.add(Identifier::random());
   ids.add(Identifier::random());
   ids.clear();
   EXPECT_EQ(ids, empty);
   EXPECT_TRUE(ids.empty());
}

TEST(IdentifiersTest, collectionIsASet)
{
   Identifiers one;
   Identifiers three;
   auto id = Identifier::random();
   three.add(id);
   three.add(id);
   three.add(id);
   one.add(id);
   EXPECT_EQ(one, three);
}

TEST(IdentifiersTest, contains)
{
   Identifier id1 = Identifier::random();
   Identifier id2 = Identifier::random();
   Identifier id3 = Identifier::random();
   Identifiers a;
   Identifiers b;
   Identifiers c;

   a.add(id1);
   b.add(id1);
   b.add(id2);
   c.add(id3);

   EXPECT_TRUE(a.contains(a));
   EXPECT_TRUE(a.contains(id1));
   EXPECT_FALSE(a.contains(id2));

   EXPECT_TRUE(b.contains(a));
   EXPECT_TRUE(b.contains(id1));
   EXPECT_TRUE(b.contains(id2));

   EXPECT_FALSE(a.contains(b));
   EXPECT_FALSE(a.contains(c));
   EXPECT_FALSE(b.contains(c));

   EXPECT_TRUE(a.contains(Identifiers {}));
   EXPECT_TRUE(Identifiers {}.contains(Identifiers {}));
}
