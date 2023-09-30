#include <gtest/gtest.h>

#include "contomap/model/Contomap.h"
#include "contomap/model/Filter.h"

using contomap::model::Contomap;
using contomap::model::Topic;

class AllTopicsFilter : public contomap::model::Filter<Topic>
{
   [[nodiscard]] bool matches([[maybe_unused]] Topic const &topic, [[maybe_unused]] contomap::model::ContomapView const &view) const override
   {
      return true;
   }
};

TEST(ContomapTest, emptyMapHasADefaultScope)
{
   Contomap map = Contomap::newMap();
   auto topic = map.findTopic(map.getDefaultScope());
   EXPECT_TRUE(topic.has_value());
}

TEST(ContomapTest, topicsCanBeIterated)
{
   Contomap map = Contomap::newMap();

   static_cast<void>(map.newTopic());
   static_cast<void>(map.newTopic());

   // TODO consider using ContomapViewFixture

   auto topics = std::ranges::common_view { map.find(std::make_shared<AllTopicsFilter>()) };
   size_t count = std::count_if(topics.begin(), topics.end(), [](auto const &) { return true; });
   EXPECT_EQ(3, count);
}
