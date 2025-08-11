#pragma once

#include <ranges>

#include <gmock/gmock.h>

#include "contomap/model/Topic.h"

namespace contomap::test::matchers
{

MATCHER_P(hasNameCountOf, expected, "")
{
   contomap::model::Topic const &topic = arg;
   auto names = topic.allNames();
   auto distance = std::ranges::distance(names.begin(), names.end());
   return distance == static_cast<decltype(distance)>(expected);
}

MATCHER_P(hasName, expected, "")
{
   contomap::model::Topic const &topic = arg;
   std::string const &expectedName(expected);
   auto namesView = std::ranges::common_view { topic.allNames() };
   return std::any_of(
      namesView.begin(), namesView.end(), [&expectedName](contomap::model::TopicName const &name) { return name.getValue().raw() == expectedName; });
}

MATCHER_P(hasDefaultName, expected, "")
{
   contomap::model::Topic const &topic = arg;
   std::string const &expectedName(expected);
   for (contomap::model::TopicName const &name : topic.allNames())
   {
      if (name.scopeEquals({}))
      {
         return name.getValue().raw() == expectedName;
      }
   }
   return false;
}

MATCHER_P2(hasScopedName, scope, expected, "")
{
   contomap::model::Topic const &topic = arg;
   std::string const &expectedName(expected);
   for (contomap::model::TopicName const &name : topic.allNames())
   {
      if (name.scopeEquals(scope))
      {
         return name.getValue().raw() == expectedName;
      }
   }
   return false;
}

}
