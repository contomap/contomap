#include <ranges>

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

TEST(GeneratorTest, rangeCompatibility)
{
   // requirements for std::ranges::range
   static_assert(std::weakly_incrementable<Generator<int>::Iterator>);
   // requirements for std::ranges::view
   static_assert(!std::copy_constructible<Generator<int>>);
   static_assert(!std::copyable<Generator<int>>);
   static_assert(std::ranges::enable_view<Generator<int>>);
   static_assert(std::ranges::range<Generator<int>>);
   // requirements for std::ranges::range
   static_assert(std::movable<Generator<int>>);
   static_assert(std::copyable<Generator<int>::Iterator>);
}
