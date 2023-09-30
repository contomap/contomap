#pragma once

#include <ranges>

#include <gmock/gmock.h>

#include "contomap/model/Topic.h"

namespace contomap::test::matchers
{

MATCHER_P(hasName, expected, "")
{
   contomap::model::Topic const &topic = arg;
   std::string const &expectedName(expected);
   auto namesView = std::ranges::common_view { topic.allNames() };
   return std::any_of(
      namesView.begin(), namesView.end(), [&expectedName](contomap::model::TopicName const &name) { return name.getValue().raw() == expectedName; });
}

}
