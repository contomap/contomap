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
   [[nodiscard]] virtual contomap::infrastructure::Search<contomap::model::Topic> find(std::shared_ptr<Filter<contomap::model::Topic>> filter) const = 0;

   /**
    * Find a topic with a specific identifier.
    *
    * @param id the identifier to search for.
    * @return a reference to the matching topic, if existing.
    */
   [[nodiscard]] virtual std::optional<std::reference_wrapper<Topic const>> findTopic(contomap::model::Identifier id) const = 0;

   /**
    * Find associations that match a certain filter.
    *
    * The returned search object will yield all associations for which the filter returns true.
    *
    * @param filter the filter to call.
    * @return a Search instance that can be iterated once.
    */
   [[nodiscard]] virtual contomap::infrastructure::Search<contomap::model::Association> find(std::shared_ptr<Filter<contomap::model::Association>> filter) const
      = 0;

   /**
    * Find a association with a specific identifier.
    *
    * @param id the identifier to search for.
    * @return a reference to the matching association, if existing.
    */
   [[nodiscard]] virtual std::optional<std::reference_wrapper<Association const>> findAssociation(contomap::model::Identifier id) const = 0;
};

}