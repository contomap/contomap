#pragma once

#include <memory>

#include "contomap/model/Association.h"
#include "contomap/model/Filter.h"

namespace contomap::model
{

class ContomapView;

/**
 * Associations provides factory functions for common operations.
 */
class Associations
{
public:
   Associations() = delete;

   /**
    * Factory function for creating a Filter for associations.
    *
    * @param scope the view scope to filter for.
    * @return a Filter instance that matches all associations with a least one occurrence in given scope.
    */
   static std::unique_ptr<contomap::model::Filter<contomap::model::Association>> thatAreIn(contomap::model::Identifiers const &scope);
};

}
