#include <gtest/gtest.h>

#include "contomap/frontend/Names.h"
#include "contomap/model/Topic.h"
#include "contomap/model/TopicNameValue.h"

#include "contomap/test/samples/TopicNameSamples.h"

using contomap::frontend::Names;
using contomap::model::Identifier;
using contomap::model::Topic;
using contomap::model::TopicNameValue;

using contomap::test::samples::someNameValue;

TEST(NamesTest, defaultIfNoneApplies)
{
   Topic topic(Identifier::random());
   auto result = Names::forDisplay(topic, Identifier::random());
   std::vector<std::string> expected { "???" };
   EXPECT_EQ(result, expected);
}

TEST(NamesTest, homeIfDefaultScope)
{
   Topic topic(Identifier::random());
   auto result = Names::forDisplay(topic, topic.getId());
   std::vector<std::string> expected { "---" };
   EXPECT_EQ(result, expected);
}

TEST(NamesTest, listedNames)
{
   Topic topic(Identifier::random());
   static_cast<void>(topic.newName(someNameValue()));
   static_cast<void>(topic.newName(someNameValue()));
   auto result = Names::forDisplay(topic, Identifier::random());
   EXPECT_EQ(result.size(), 2);
}
