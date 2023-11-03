#include <gtest/gtest.h>

#include "contomap/frontend/Names.h"
#include "contomap/model/Topic.h"
#include "contomap/model/TopicNameValue.h"

#include "contomap/test/samples/TopicNameSamples.h"
#include "contomap/test/samples/TopicSamples.h"

using contomap::frontend::Names;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Topic;
using contomap::model::TopicNameValue;

using contomap::test::samples::named;
using contomap::test::samples::someNameValue;
using contomap::test::samples::someTopic;

static Identifiers someScope()
{
   return Identifiers::ofSingle(Identifier::random());
}

TEST(NamesTest, defaultIfNoneApplies)
{
   Topic topic(Identifier::random());
   auto result = Names::forDisplay(topic, Identifier::random());
   std::vector<std::string> expected { "???" };
   EXPECT_EQ(result, expected);
}

TEST(NamesTest, homeIfDefaultScope)
{
   auto topic = someTopic();
   auto result = Names::forDisplay(topic, topic.getId());
   std::vector<std::string> expected { "---" };
   EXPECT_EQ(result, expected);
}

TEST(NamesTest, listedNames)
{
   auto topic = someTopic();
   static_cast<void>(topic.newName(someScope(), someNameValue()));
   static_cast<void>(topic.newName(someScope(), someNameValue()));
   auto result = Names::forDisplay(topic, Identifier::random());
   EXPECT_EQ(result.size(), 2);
}

TEST(Namestest, scopedNames)
{
   auto topic = someTopic();
   auto idA = Identifier::random();
   auto idB = Identifier::random();
   auto idC = Identifier::random();
   auto idD = Identifier::random();

   Identifiers scopeA = Identifiers::ofSingle(idA);
   Identifiers scopeB = Identifiers::ofSingle(idB);
   Identifiers scopeAB;
   scopeAB.add(idA);
   scopeAB.add(idB);
   Identifiers scopeBC;
   scopeBC.add(idB);
   scopeBC.add(idC);
   Identifiers scopeABC;
   scopeABC.add(idA);
   scopeABC.add(idB);
   scopeABC.add(idC);
   Identifiers scopeD;
   scopeD.add(idD);

   static_cast<void>(topic.newName(scopeA, named("nameA")));
   static_cast<void>(topic.newName(scopeB, named("nameB")));
   static_cast<void>(topic.newName(scopeAB, named("nameAB")));
   static_cast<void>(topic.newName(scopeBC, named("nameBC")));

   auto resultB = Names::forScopedDisplay(topic, scopeB, Identifier::random());
   std::vector<std::string> expectedB { "nameB" };
   EXPECT_EQ(resultB, expectedB) << "wrong for scopeB";

   auto resultABC = Names::forScopedDisplay(topic, scopeABC, Identifier::random());
   std::vector<std::string> expectedABC { "nameAB", "nameBC" };
   EXPECT_EQ(std::set<std::string>(resultABC.begin(), resultABC.end()), std::set<std::string>(expectedABC.begin(), expectedABC.end())) << "wrong for scopeABC";

   auto resultD = Names::forScopedDisplay(topic, scopeD, Identifier::random());
   std::vector<std::string> expectedD { "???" };
   EXPECT_EQ(resultD, expectedD) << "scopeD should default to question marks";
}
