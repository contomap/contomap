#include "contomap/model/Topics.h"

using contomap::model::Filter;
using contomap::model::Identifiers;
using contomap::model::Topic;
using contomap::model::Topics;

std::unique_ptr<Filter<Topic>> Topics::thatAreIn(Identifiers const &scope)
{
   return Filter<Topic>::of([scope](Topic const &topic, [[maybe_unused]] ContomapView const &view) { return topic.isIn(scope); });
}
