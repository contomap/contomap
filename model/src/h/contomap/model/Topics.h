#pragma once

#include <memory>

#include "contomap/model/Filter.h"
#include "contomap/model/Topic.h"

namespace contomap::model
{

class ContomapView;

/**
 * Topics provides factory functions for common operations.
 */
class Topics
{
public:
   Topics() = delete;

   /**
    * Factory function for creating a Filter for topics.
    *
    * @param scope the view scope to filter for.
    * @return a Filter instance that matches all topics with a least one occurrence in given scope.
    */
   static std::unique_ptr<contomap::model::Filter<contomap::model::Topic>> thatAreIn(contomap::model::Identifiers const &scope);
};

}
