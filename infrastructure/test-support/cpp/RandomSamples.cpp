#include <random>

#include "contomap/test/samples/RandomSamples.h"

static thread_local std::mt19937 gen([]() {
   std::random_device rd;
   return rd();
}());

namespace contomap::test::samples
{

size_t randomIndexOf(size_t limit)
{
   std::uniform_int_distribution<size_t> dist(0, limit - 1);
   return dist(gen);
}

float randomSignedFraction()
{
   std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
   return dist(gen);
}

}
