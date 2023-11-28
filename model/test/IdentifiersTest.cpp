#include <gtest/gtest.h>

#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/Identifiers.h"

using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
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
   EXPECT_NE(one, empty) << "identifiers can't be equal to empty collection if something was added";
   EXPECT_FALSE(one.empty()) << "identifiers can't be empty if something was added";
   EXPECT_TRUE(one.remove(id)) << "removal should return true if previously present";
   EXPECT_EQ(one, empty) << "identifiers should be equal to empty collection after removing single element";
   EXPECT_TRUE(one.empty()) << "identifiers should be empty if single element was removed";
   EXPECT_FALSE(one.remove(Identifier::random())) << "removal should return false if unknown was removed";
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

TEST(IdentifiersTest, size)
{
   Identifiers ids;
   EXPECT_EQ(0, ids.size());
   ids.add(Identifier::random());
   ids.add(Identifier::random());
   ids.add(Identifier::random());
   EXPECT_EQ(3, ids.size());
   ids.clear();
   EXPECT_EQ(0, ids.size());
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

static void testCode(std::string const &testCase, Identifiers const &source)
{
   Encoder encoder;
   Identifiers encodeCopy = source;
   encodeCopy.code(testCase, encoder);
   auto &data = encoder.getData();
   Decoder decoder(data.data(), data.data() + data.size());
   Identifiers clone;
   try
   {
      clone.code(testCase, decoder);
   }
   catch (std::exception &)
   {
   }
   EXPECT_EQ(source, clone) << "Failed for " << testCase;
}

TEST(IdentifiersTest, serialization)
{
   testCode("empty", Identifiers {});
   testCode("single", Identifiers::ofSingle(Identifier::random()));
   Identifiers multiple;
   multiple.add(Identifier::random());
   multiple.add(Identifier::random());
   multiple.add(Identifier::random());
   multiple.add(Identifier::random());
   testCode("multiple", multiple);
}
