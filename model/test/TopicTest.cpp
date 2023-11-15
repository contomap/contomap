#include <gtest/gtest.h>

#include "contomap/model/Topic.h"

#include "contomap/test/samples/TopicNameSamples.h"
#include "contomap/test/samples/TopicSamples.h"

using contomap::model::Topic;

using contomap::test::samples::someNameValue;
using contomap::test::samples::someTopic;

TEST(TopicTest, namesCanBeCreated)
{
   auto topic = someTopic();
   auto nameValue1 = someNameValue();
   auto name1 = topic.newName({}, nameValue1);
   EXPECT_EQ(nameValue1, name1.getValue()) << "Value should be kept";

   auto name2 = topic.newName({}, someNameValue());
   EXPECT_NE(name1.getValue(), name2.getValue()) << "Value should be kept uniquely per instance";
}
