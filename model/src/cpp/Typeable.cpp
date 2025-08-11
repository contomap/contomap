#include "contomap/model/Topic.h"

#include "contomap/model/Typeable.h"

using contomap::model::Topic;
using contomap::model::Typeable;

Typeable::~Typeable()
{
   clearType();
}

void Typeable::setType(Topic &topic)
{
   clearType();
   type = topic.linkTyped([this]() { type.reset(); });
}

bool Typeable::hasType() const
{
   return !hasNoType();
}

std::optional<std::reference_wrapper<Topic>> Typeable::getType()
{
   if (hasNoType())
   {
      return {};
   }
   return { type->getLinked() };
}

std::optional<std::reference_wrapper<Topic const>> Typeable::getType() const
{
   if (hasNoType())
   {
      return {};
   }
   return { type->getLinked() };
}

void Typeable::clearType()
{
   if (hasNoType())
   {
      return;
   }
   auto old = std::move(type);
   old.reset();
}

void Typeable::encodeTypeable(contomap::infrastructure::serial::Encoder &coder) const
{
   contomap::infrastructure::serial::Coder::Scope scope(coder, "type");
   uint8_t marker = hasType() ? 0x01 : 0x00;
   coder.code("present", marker);
   if (marker != 0x00)
   {
      type->getLinked().getId().encode(coder, "id");
   }
}

void Typeable::decodeTypeable(contomap::infrastructure::serial::Decoder &coder, std::function<Topic &(contomap::model::Identifier)> const &resolver)
{
   contomap::infrastructure::serial::Coder::Scope scope(coder, "type");
   uint8_t marker = 0x00;
   coder.code("present", marker);
   if (marker != 0x00)
   {
      auto id = contomap::model::Identifier::from(coder, "id");
      setType(resolver(id));
   }
}

bool Typeable::hasNoType() const
{
   return type == nullptr;
}
