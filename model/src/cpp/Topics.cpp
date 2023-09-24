#include "contomap/model/Topics.h"

using contomap::model::Identifiers;
using contomap::model::Topic;
using contomap::model::TopicFilter;
using contomap::model::Topics;

std::unique_ptr<TopicFilter> Topics::thatAreIn(Identifiers const &scope)
{
   return TopicFilter::of([scope](Topic const &topic, [[maybe_unused]] ContomapView const &view) { return topic.isIn(scope); });
}
