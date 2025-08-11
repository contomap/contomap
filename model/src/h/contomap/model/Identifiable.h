#pragma once

#include "contomap/model/Identifier.h"

namespace contomap::model
{

/**
 * An Identifiable is something that has an identifier.
 */
class Identifiable
{
public:
   /**
    * @return the unique identifier of this instance.
    */
   [[nodiscard]] virtual contomap::model::Identifier getId() const = 0;

protected:
   virtual ~Identifiable() = default;
};

}
