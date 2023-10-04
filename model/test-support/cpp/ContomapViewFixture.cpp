#include <gmock/gmock.h>

#include "contomap/test/fixtures/ContomapViewFixture.h"

using contomap::model::Association;
using contomap::model::ContomapView;
using contomap::model::Filter;
using contomap::model::Identifier;
using contomap::model::Topic;

using contomap::test::fixtures::ContomapViewFixture;

ContomapViewFixture::ContomapViewFixture(ContomapView const &view)
   : view(view)
{
}

void ContomapViewFixture::shouldHaveTopicCountOf(size_t expected) const
{
   auto topics = std::ranges::common_view(view.find(Filter<Topic>::of([](Topic const &, ContomapView const &) { return true; })));
   size_t count = std::count_if(topics.begin(), topics.end(), [](auto const &) { return true; });
   EXPECT_EQ(expected, count) << "Wrong number of topics";
}

void ContomapViewFixture::shouldNotHaveTopic(Identifier id) const
{
   EXPECT_FALSE(view.findTopic(id).has_value()) << "Topic found although shouldn't be present";
}

void ContomapViewFixture::shouldHaveTopicThat(Identifier id, std::function<void(Topic const &)> const &asserter) const
{
   auto topic = view.findTopic(id);
   if (!topic.has_value())
   {
      FAIL() << "Topic with ID " << id << " not found";
   }
   asserter(topic.value());
}

void ContomapViewFixture::shouldHaveAssociationCountOf(size_t expected) const
{
   auto views = std::ranges::common_view(view.find(Filter<Association>::of([](Association const &, ContomapView const &) { return true; })));
   size_t count = std::count_if(views.begin(), views.end(), [](auto const &) { return true; });
   EXPECT_EQ(expected, count) << "Wrong number of associations";
}

void ContomapViewFixture::shouldHaveAssociationThat(Identifier id, std::function<void(Association const &)> const &asserter) const
{
   auto association = view.findAssociation(id);
   if (!association.has_value())
   {
      FAIL() << "Association with ID " << id << " not found";
   }
   asserter(association.value());
}

void ContomapViewFixture::shouldHaveOneAssociationNear(contomap::model::SpacialCoordinate position) const
{
   auto associations = view.find(Filter<Association>::of([&position](Association const &association, ContomapView const &) {
      auto const &expectedPoint = position.getAbsoluteReference();
      auto const &argPoint = association.getLocation().getSpacial().getAbsoluteReference();
      return (std::abs(argPoint.X() - expectedPoint.X()) < 0.001f) && (std::abs(argPoint.Y() - expectedPoint.Y()) < 0.001f);
   }));
   auto associationsView = std::ranges::common_view(std::move(associations));
   size_t count = std::count_if(associationsView.begin(), associationsView.end(), [](auto const &) { return true; });
   EXPECT_EQ(1, count);
}
