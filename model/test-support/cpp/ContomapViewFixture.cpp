#include <gmock/gmock.h>

#include "contomap/test/fixtures/ContomapViewFixture.h"

using contomap::model::ContomapView;
using contomap::model::Topic;

using contomap::test::fixtures::ContomapViewFixture;

class AllTopicsFilter : public contomap::model::Filter<Topic>
{
   [[nodiscard]] bool matches([[maybe_unused]] Topic const &topic, [[maybe_unused]] contomap::model::ContomapView const &view) const override
   {
      return true;
   }
};

ContomapViewFixture::ContomapViewFixture(ContomapView const &view)
   : view(view)
{
}

void ContomapViewFixture::shouldHaveTopicCountOf(size_t expected)
{
   auto gen = view.find(std::make_shared<AllTopicsFilter>());

   size_t count = 0;
   for ([[maybe_unused]] Topic const &topic : gen)
   {
      count++;
   }
   EXPECT_EQ(expected, count);
}

void ContomapViewFixture::shouldHaveTopicThat(contomap::model::Identifier id, std::function<void(contomap::model::Topic const &)> const &asserter)
{
   auto topic = view.findTopic(id);
   if (!topic.has_value())
   {
      FAIL() << "Topic with ID " << id << " not found";
   }
   asserter(topic.value());
}

void ContomapViewFixture::shouldHaveAssociationThat(contomap::model::Identifier id, std::function<void(contomap::model::Association const &)> const &asserter)
{
   auto association = view.findAssociation(id);
   if (!association.has_value())
   {
      FAIL() << "Association with ID " << id << " not found";
   }
   asserter(association.value());
}
