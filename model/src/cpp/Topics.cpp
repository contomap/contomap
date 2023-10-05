#include <string.h>

#include "contomap/model/Topics.h"

using contomap::model::Filter;
using contomap::model::Identifiers;
using contomap::model::Topic;
using contomap::model::Topics;

std::unique_ptr<Filter<Topic>> Topics::thatAreIn(Identifiers const &scope)
{
   return Filter<Topic>::of([scope](Topic const &topic, ContomapView const &) { return topic.isIn(scope); });
}

std::unique_ptr<Filter<Topic>> Topics::thatOccurAs(Identifiers const &occurrences)
{
   return Filter<Topic>::of([occurrences](Topic const &topic, ContomapView const &) { return topic.occursAsAnyOf(occurrences); });
}

std::unique_ptr<Filter<Topic>> Topics::withANameLike(std::string const &searchValue)
{
   auto lower = [](std::string text) {
      std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return std::tolower(c); });
      return text;
   };
   auto nameMatcher = [lowerSearchValue = lower(searchValue), lower](TopicName const &name) {
      auto nameValue = lower(name.getValue().raw());
      if (lowerSearchValue.empty() || nameValue.empty())
      {
         return false;
      }
      return ::strstr(nameValue.c_str(), lowerSearchValue.c_str()) != nullptr;
   };
   return Filter<Topic>::of([nameMatcher](Topic const &topic, ContomapView const &) {
      auto names = std::ranges::common_view(topic.allNames());
      return std::any_of(names.begin(), names.end(), nameMatcher);
   });
}
