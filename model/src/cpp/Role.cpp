#include "contomap/model/Role.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Identifier;
using contomap::model::OptionalIdentifier;
using contomap::model::Role;
using contomap::model::Style;

Role::Role(Seed const &seed)
   : id(seed.id)
   , parent(seed.parent)
{
}

std::unique_ptr<Role> Role::from(contomap::infrastructure::serial::Decoder &coder, uint8_t version, contomap::model::Identifier id,
   std::function<std::optional<std::reference_wrapper<Topic>>(contomap::model::Identifier)> const &,
   std::function<std::optional<std::reference_wrapper<Association>>(contomap::model::Identifier)> const &)
{
   Coder::Scope scope(coder, "role");
   auto parent = Identifier::from(coder, "parent");
   // TODO: throw if associationResolver can not find parent
   auto role = std::make_unique<Role>(Seed(id, parent));
   role->type = OptionalIdentifier::from(coder, "type");
   // TODO: throw if topicResolver can not find parent
   role->appearance.decode(coder, "appearance", version);
   return role;
}

void Role::encode(contomap::infrastructure::serial::Encoder &coder) const
{
   Coder::Scope scope(coder, "role");
   parent.encode(coder, "parent");
   type.encode(coder, "type");
   appearance.encode(coder, "appearance");
}

Identifier Role::getId() const
{
   return id;
}

Identifier Role::getParent() const
{
   return parent;
}

void Role::setAppearance(Style style)
{
   appearance = std::move(style);
}

Style Role::getAppearance() const
{
   return appearance;
}

void Role::setType(Identifier typeTopicId)
{
   type = typeTopicId;
}

void Role::clearType()
{
   type.clear();
}

OptionalIdentifier Role::getType() const
{
   return type;
}
