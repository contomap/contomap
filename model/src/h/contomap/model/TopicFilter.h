#pragma once

#include <functional>
#include <memory>

#include "contomap/model/Topic.h"

namespace contomap::model
{

class ContomapView;

/**
 * TopicFilter is used to query topics with specific properties.
 */
class TopicFilter
{
public:
   /** Function is a functional representation of a filter. */
   using Function = std::function<bool(contomap::model::Topic const &, contomap::model::ContomapView const &)>;

   virtual ~TopicFilter() = default;

   /**
    * Factory function for creating a TopicFilter that is based on a function.
    * Use this variant to create a filter in case the filter can be expressed as a lambda.
    *
    * @param fn the function to call for each item.
    * @return a TopicFilter based on provided function.
    */
   [[nodiscard]] static std::unique_ptr<TopicFilter> of(Function fn);

   /**
    * Test whether a specific topic is passing the filter.
    *
    * @param topic the topic to test.
    * @param view the view within to verify indirect properties.
    * @return true in case the topic matches (passes) the filter, false otherwise.
    */
   [[nodiscard]] virtual bool matches(contomap::model::Topic const &topic, contomap::model::ContomapView const &view) const = 0;
};

}
