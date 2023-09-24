#include <gtest/gtest.h>

#include "contomap/model/Contomap.h"
#include "contomap/model/TopicFilter.h"

using contomap::model::Contomap;
using contomap::model::Topic;

class AllTopicsFilter : public contomap::model::TopicFilter
{
   [[nodiscard]] bool matches([[maybe_unused]] contomap::model::Topic const &topic, [[maybe_unused]] contomap::model::ContomapView const &view) const override
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

   auto gen = map.findTopics(std::make_shared<AllTopicsFilter>());

   size_t count = 0;
   for ([[maybe_unused]] Topic const &topic : gen)
   {
      count++;
   }
   EXPECT_EQ(3, count);
}
