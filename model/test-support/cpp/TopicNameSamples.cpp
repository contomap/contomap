#include <atomic>
#include <string>

#include "contomap/test/samples/TopicNameSamples.h"

using contomap::model::TopicNameValue;

TopicNameValue contomap::test::samples::someNameValue()
{
   static std::atomic<uint32_t> nameCounter(0);
   return std::get<TopicNameValue>(TopicNameValue::from(std::string("name") + std::to_string(nameCounter++)));
}
