#include "contomap/model/OptionalIdentifier.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
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

OptionalIdentifier OptionalIdentifier::from(Decoder &coder, std::string const &name)
{
   Coder::Scope scope(coder, name);
   uint8_t present = 0;
   coder.code("present", present);
   return (present != 0) ? of(Identifier::from(coder, "id")) : OptionalIdentifier();
}

void OptionalIdentifier::encode(Encoder &coder, std::string const &name) const
{
   Coder::Scope scope(coder, name);
   uint8_t present = isAssigned() ? 1 : 0;
   coder.code("present", present);
   if (present != 0)
   {
      value().encode(coder, "id");
   }
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
