#pragma once

#include <memory>

#include "contomap/model/Filter.h"

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
    * Factory function for creating a TopicFilter.
    *
    * @param scope the view scope to filter for.
    * @return a TopicFilter instance that matches all topics with a least one occurrence in given scope.
    */
   static std::unique_ptr<contomap::model::Filter<Topic>> thatAreIn(contomap::model::Identifiers const &scope);
};

}
