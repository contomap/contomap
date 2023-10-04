#pragma once

#include <cstddef>

namespace contomap::test::samples
{

/**
 * @return a random value within given limit.
 */
[[nodiscard]] size_t randomIndexOf(size_t limit);

/**
 * @return a random floating point value in the range of -1.0 to +1.0 (inclusive, each).
 */
[[nodiscard]] float randomSignedFraction();

}
