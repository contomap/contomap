#pragma once

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
   virtual ~TopicFilter() = default;

   /**
    * Test whether a specific topic is passing the filter.
    *
    * @param topic the topic to test.
    * @param view the view within to verify indirect properties.
    * @return true in case the topic matches (passes) the filter, false otherwise.
    */
   [[nodiscard]] virtual bool matches(contomap::model::Topic const &topic, contomap::model::ContomapView const &view) = 0;
};

}
