#include "contomap/model/Role.h"
#include "contomap/model/Association.h"
#include "contomap/model/Topic.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Association;
using contomap::model::Identifier;
using contomap::model::OptionalIdentifier;
using contomap::model::Role;
using contomap::model::Style;
using contomap::model::Topic;

Role::Role(Identifier id, Topic &topic, Association &association)
   : id(id)
   , topic(topic.link(*this, [this]() { unlink(); }))
   , association(association.link(*this, [this]() { unlink(); }))
{
}

std::unique_ptr<Role> Role::from(contomap::infrastructure::serial::Decoder &coder, uint8_t version, contomap::model::Identifier id,
   std::function<Topic &(contomap::model::Identifier)> const &topicResolver,
   std::function<Association &(contomap::model::Identifier)> const &associationResolver)
{
   Coder::Scope scope(coder, "role");
   auto topicId = Identifier::from(coder, "topic");
   auto associationId = Identifier::from(coder, "association");

   auto role = std::make_unique<Role>(id, topicResolver(topicId), associationResolver(associationId));
   role->decodeTypeable(coder, topicResolver);
   role->appearance.decode(coder, "appearance", version);
   role->decodeReifiable(coder, topicResolver);
   return role;
}

void Role::encode(contomap::infrastructure::serial::Encoder &coder) const
{
   Coder::Scope scope(coder, "role");
   topic->getLinked().getId().encode(coder, "topic");
   association->getLinked().getId().encode(coder, "association");
   encodeTypeable(coder);
   appearance.encode(coder, "appearance");
   encodeReifiable(coder);
}

Identifier Role::getId() const
{
   return id;
}

Identifier Role::getParent() const
{
   return association->getLinked().getId();
}

void Role::setAppearance(Style style)
{
   appearance = std::move(style);
}

Style Role::getAppearance() const
{
   return appearance;
}

void Role::unlink()
{
   association.reset();
   topic.reset();
}
