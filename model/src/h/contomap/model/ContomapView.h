#pragma once

#include <memory>

#include "contomap/infrastructure/Generator.h"
#include "contomap/model/Association.h"
#include "contomap/model/Filter.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Style.h"
#include "contomap/model/Topic.h"

namespace contomap::model
{

/**
 * ContomapView provides read-only access to a Contomap.
 */
class ContomapView
{
public:
   virtual ~ContomapView() = default;

   /**
    * Find topics that match a certain filter.
    *
    * The returned search object will yield all topics for which the filter returns true.
    *
    * @param filter the filter to call.
    * @return a Search instance that can be iterated once.
    */
   [[nodiscard]] virtual contomap::infrastructure::Search<contomap::model::Topic> findTopics(std::shared_ptr<Filter<contomap::model::Topic>> filter) const = 0;

   [[nodiscard]] virtual std::optional<std::reference_wrapper<Topic const>> findTopic(contomap::model::Identifier id) const = 0;
};

}
