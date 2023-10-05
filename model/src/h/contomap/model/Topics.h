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

   /**
    * Factory function for creating a Filter for topics.
    *
    * @param occurrences the identifiers of occurrences that the topics should have.
    * @return a Filter instance that matches all topics with a least one occurrence in given list.
    */
   static std::unique_ptr<contomap::model::Filter<contomap::model::Topic>> thatOccurAs(contomap::model::Identifiers const &occurrences);

   /**
    * Factory function for creating a Filter for topics.
    *
    * @param searchValue the content to look for.
    * @return a Filter instance that matches all topics with a name that matches the given value.
    */
   static std::unique_ptr<contomap::model::Filter<contomap::model::Topic>> withANameLike(std::string const &searchValue);
};

}
