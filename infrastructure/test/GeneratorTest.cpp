#include <gtest/gtest.h>

#include "contomap/infrastructure/Generator.h"

using contomap::infrastructure::Generator;

static Generator<int> range(int first, int last)
{
   while (first < last)
   {
      co_yield first++;
   }
}

TEST(GeneratorTest, canIterateOverResults)
{
   std::vector<int> result;
   for (int const value : range(10, 14))
   {
      result.emplace_back(value);
   }
   std::vector<int> expected { 10, 11, 12, 13 };
   EXPECT_EQ(expected, result);
}
