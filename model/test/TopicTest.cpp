#include <gmock/gmock.h>

#include "contomap/model/Topic.h"

#include "contomap/test/matchers/Coordinates.h"
#include "contomap/test/printers/model.h"
#include "contomap/test/samples/CoordinateSamples.h"
#include "contomap/test/samples/ScopeSamples.h"
#include "contomap/test/samples/TopicNameSamples.h"
#include "contomap/test/samples/TopicSamples.h"

using contomap::model::Topic;

using contomap::test::matchers::isCloseTo;
using contomap::test::samples::someNameValue;
using contomap::test::samples::someNonEmptyScope;
using contomap::test::samples::someSpacialCoordinate;
using contomap::test::samples::someTopic;

using testing::NiceMock;

class TopicTest : public ::testing::Test
{
public:
   class MockedReified : public contomap::model::Reified
   {
   public:
      MOCK_METHOD0(clearReifier, void());
   };
};

TEST_F(TopicTest, namesCanBeCreated)
{
   auto topic = someTopic();
   auto nameValue1 = someNameValue();
   auto name1 = topic.newName({}, nameValue1);
   EXPECT_EQ(nameValue1, name1.getValue()) << "Value should be kept";

   auto name2 = topic.newName({}, someNameValue());
   EXPECT_NE(name1.getValue(), name2.getValue()) << "Value should be kept uniquely per instance";
}

TEST_F(TopicTest, clearingReifiedWithoutHavingOneDoesNothing)
{
   auto topic = someTopic();
   topic.clearReified();
}

TEST_F(TopicTest, clearingReifiedCallsSetReified)
{
   NiceMock<MockedReified> reified;
   auto topic = someTopic();
   topic.setReified(reified);
   EXPECT_CALL(reified, clearReifier());
   topic.clearReified();
}

TEST_F(TopicTest, settingNewReifiedClearsOldOne)
{
   NiceMock<MockedReified> reified1;
   NiceMock<MockedReified> reified2;
   auto topic = someTopic();
   topic.setReified(reified1);
   EXPECT_CALL(reified1, clearReifier());
   topic.setReified(reified2);
}

TEST_F(TopicTest, clearingReifiedGuardsAgainstCallback)
{
   NiceMock<MockedReified> reified;
   auto topic = someTopic();
   size_t times = 0;
   topic.setReified(reified);
   ON_CALL(reified, clearReifier()).WillByDefault(testing::Invoke([&topic, &times]() {
      times++;
      if (times == 1)
      {
         topic.clearReified();
      }
   }));
   topic.clearReified();
   EXPECT_EQ(times, 1);
}

TEST_F(TopicTest, occurrenceBaseProperties)
{
   auto topic = someTopic();
   auto scope = someNonEmptyScope();
   auto position = someSpacialCoordinate();
   auto &occurrence = topic.newOccurrence(scope, position);
   EXPECT_EQ(&topic, &occurrence.getTopic());
   EXPECT_TRUE(occurrence.isIn(scope));
   EXPECT_FALSE(occurrence.isIn({}));
   EXPECT_THAT(occurrence.getLocation().getSpacial(), isCloseTo(position));
}
