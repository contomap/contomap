#pragma once

#include "contomap/model/TopicName.h"

namespace contomap::test::samples
{

/**
 * @return a valid random name value for topics.
 */
[[nodiscard]] contomap::model::TopicNameValue someNameValue();

/**
 * Creates a topic name value that wraps given string.
 * This function throws in case the value is empty.
 *
 * @param value the string to wrap.
 * @return a name value for topics.
 */
[[nodiscard]] contomap::model::TopicNameValue named(std::string const &value);

}
