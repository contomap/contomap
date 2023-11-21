#include <sstream>

#include <gtest/gtest.h>

#include "contomap/model/OptionalIdentifier.h"

using contomap::model::Identifier;
using contomap::model::OptionalIdentifier;

template <class T> static std::string stringOf(T const &value)
{
   std::ostringstream buf;
   buf << value;
   return buf.str();
}

TEST(OptionalIdentifierTest, defaultConstructedIsNotAssigned)
{
   OptionalIdentifier id;

   EXPECT_FALSE(id.isAssigned());
}

TEST(OptionalIdentifierTest, canBeAssignedFromRaw)
{
   OptionalIdentifier id;

   auto source = Identifier::random();
   id = source;
   EXPECT_TRUE(id.isAssigned());
   EXPECT_EQ(id.value(), source);
}

TEST(OptionalIdentifierTest, canBeAssignedFromCopy)
{
   Identifier rawA = Identifier::random();
   Identifier rawB = Identifier::random();
   OptionalIdentifier id1;
   OptionalIdentifier id2;
   OptionalIdentifier id3;

   id1 = rawA;
   id2 = rawB;
   id3 = OptionalIdentifier::of(rawA);
   EXPECT_NE(id1, id2);
   EXPECT_EQ(id1, id3);
}

TEST(OptionalIdentifierTest, stringRepresentation)
{
   EXPECT_EQ("------------", stringOf(OptionalIdentifier {}));

   Identifier id = Identifier::random();
   EXPECT_EQ(stringOf(id), stringOf(OptionalIdentifier::of(id)));
}

TEST(OptionalIdentifierTest, clearing)
{
   auto id = OptionalIdentifier::of(Identifier::random());
   EXPECT_TRUE(id.isAssigned());
   id.clear();
   EXPECT_FALSE(id.isAssigned());
   EXPECT_EQ(OptionalIdentifier {}, id);
}
