#pragma once

#include <memory>

#include "contomap/infrastructure/Generator.h"
#include "contomap/model/Association.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Style.h"
#include "contomap/model/Topic.h"

namespace contomap::model
{

class TopicFilter;

/**
 * ContomapView provides read-only access to a Contomap.
 */
class ContomapView
{
public:
   virtual ~ContomapView() = default;

   using Search = contomap::infrastructure::Generator<std::reference_wrapper<contomap::model::Topic const>>;

   [[nodiscard]] virtual Search findTopics(std::shared_ptr<TopicFilter> filter) const = 0;
};

}
