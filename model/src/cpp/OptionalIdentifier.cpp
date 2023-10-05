#include "contomap/model/OptionalIdentifier.h"

using contomap::model::Identifier;
using contomap::model::OptionalIdentifier;

OptionalIdentifier::OptionalIdentifier(Identifier value)
   : value(value)
{
}

OptionalIdentifier OptionalIdentifier::of(Identifier id)
{
   return OptionalIdentifier(id);
}

bool OptionalIdentifier::isAssigned() const
{
   return value.has_value();
}

void OptionalIdentifier::clear()
{
   value.reset();
}

OptionalIdentifier &OptionalIdentifier::operator=(Identifier id)
{
   value = id;
   return *this;
}
