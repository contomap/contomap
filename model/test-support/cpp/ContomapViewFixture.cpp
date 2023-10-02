#include <gmock/gmock.h>

#include "contomap/test/fixtures/ContomapViewFixture.h"

using contomap::model::Association;
using contomap::model::ContomapView;
using contomap::model::Filter;
using contomap::model::Identifier;
using contomap::model::Topic;

using contomap::test::fixtures::ContomapViewFixture;

class AllTopicsFilter : public contomap::model::Filter<Topic>
{
   [[nodiscard]] bool matches([[maybe_unused]] Topic const &topic, [[maybe_unused]] ContomapView const &view) const override
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

void ContomapViewFixture::shouldHaveTopicThat(Identifier id, std::function<void(Topic const &)> const &asserter)
{
   auto topic = view.findTopic(id);
   if (!topic.has_value())
   {
      FAIL() << "Topic with ID " << id << " not found";
   }
   asserter(topic.value());
}

void ContomapViewFixture::shouldHaveAssociationThat(Identifier id, std::function<void(Association const &)> const &asserter)
{
   auto association = view.findAssociation(id);
   if (!association.has_value())
   {
      FAIL() << "Association with ID " << id << " not found";
   }
   asserter(association.value());
}

void ContomapViewFixture::shouldHaveOneAssociationNear(contomap::model::SpacialCoordinate position)
{
   auto associations = view.find(Filter<Association>::of([&position](Association const &association, ContomapView const &) {
      auto const &expectedPoint = position.getAbsoluteReference();
      auto const &argPoint = association.getLocation().getSpacial().getAbsoluteReference();
      return (std::abs(argPoint.X() - expectedPoint.X()) < 0.001f) && (std::abs(argPoint.Y() - expectedPoint.Y()) < 0.001f);
   }));
   auto associationsView = std::ranges::common_view { std::move(associations) };
   size_t count = std::count_if(associationsView.begin(), associationsView.end(), [](auto const &) { return true; });
   EXPECT_EQ(1, count);
}
