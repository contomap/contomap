#include "contomap/model/Associations.h"

using contomap::model::Association;
using contomap::model::Associations;
using contomap::model::Filter;
using contomap::model::Identifiers;

std::unique_ptr<Filter<Association>> Associations::thatAreIn(Identifiers const &scope)
{
   return Filter<Association>::of([scope](Association const &association, [[maybe_unused]] ContomapView const &view) { return association.isIn(scope); });
}
