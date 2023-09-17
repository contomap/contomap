#include <gtest/gtest.h>

#include "contomap/model/Contomap.h"
#include "contomap/model/TopicFilter.h"

using contomap::model::Contomap;
using contomap::model::Topic;

class AllTopicsFilter : public contomap::model::TopicFilter
{
   [[nodiscard]] bool matches(contomap::model::Topic const &topic, contomap::model::ContomapView const &view) override
   {
      return true;
   }
};

TEST(ContomapTest, topicsCanBeIterated)
{
   Contomap map;

   [[maybe_unused]] auto t1 = map.newTopic();
   [[maybe_unused]] auto t2 = map.newTopic();

   auto gen = map.findTopics(std::make_shared<AllTopicsFilter>());

   size_t count = 0;
   for ([[maybe_unused]] Topic const &topic : gen)
   {
      count++;
   }
   EXPECT_EQ(2, count);
}
