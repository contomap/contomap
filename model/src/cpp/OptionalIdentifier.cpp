#include "contomap/model/OptionalIdentifier.h"

using contomap::model::Identifier;
using contomap::model::OptionalIdentifier;

OptionalIdentifier::OptionalIdentifier(Identifier value)
   : content(value)
{
}

OptionalIdentifier OptionalIdentifier::of(Identifier id)
{
   return OptionalIdentifier(id);
}

bool OptionalIdentifier::isAssigned() const
{
   return content.has_value();
}

Identifier OptionalIdentifier::value() const
{
   return content.value();
}

void OptionalIdentifier::clear()
{
   content.reset();
}

OptionalIdentifier &OptionalIdentifier::operator=(Identifier id)
{
   content = id;
   return *this;
}
