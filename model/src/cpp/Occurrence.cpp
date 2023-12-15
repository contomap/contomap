#include "contomap/model/Occurrence.h"
#include "contomap/model/Topic.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::OptionalIdentifier;
using contomap::model::Style;
using contomap::model::Topic;

Occurrence::Occurrence(Identifier id, Topic &topic, Identifiers scope, SpacialCoordinate spacial)
   : id(id)
   , topic(topic)
   , scope(std::move(scope))
   , location(spacial)
{
}

Occurrence::Occurrence(Identifier id, Topic &topic)
   : id(id)
   , topic(topic)
{
}

std::unique_ptr<Occurrence> Occurrence::from(contomap::infrastructure::serial::Decoder &coder, uint8_t version, contomap::model::Identifier id, Topic &topic,
   std::function<Topic &(contomap::model::Identifier)> const &topicResolver)
{
   Coder::Scope serialScope(coder, "occurrence");
   std::unique_ptr<Occurrence> occurrence(new Occurrence(id, topic));
   occurrence->scope.decode(coder, "scope");
   occurrence->location.decode(coder, "location", version);
   occurrence->type = OptionalIdentifier::from(coder, "type");
   // TODO: throw if topicResolver can not find type
   occurrence->appearance.decode(coder, "appearance", version);
   occurrence->decodeReifiable(coder, topicResolver);
   return occurrence;
}

void Occurrence::encode(Encoder &coder) const
{
   Coder::Scope serialScope(coder, "occurrence");
   scope.encode(coder, "scope");
   location.encode(coder, "location");
   type.encode(coder, "type");
   appearance.encode(coder, "appearance");
   encodeReifiable(coder);
}

Identifier Occurrence::getId() const
{
   return id;
}

Topic const &Occurrence::getTopic() const
{
   return topic;
}

Identifiers const &Occurrence::getScope() const
{
   return scope;
}

contomap::model::Coordinates const &Occurrence::getLocation() const
{
   return location;
}

bool Occurrence::isIn(Identifiers const &thatScope) const
{
   return thatScope.contains(scope);
}

bool Occurrence::scopeContains(Identifier thatId) const
{
   return scope.contains(thatId);
}

bool Occurrence::hasNarrowerScopeThan(Occurrence const &other) const
{
   return (scope.size() > other.scope.size()) || (hasSameScopeSizeAs(other) && (scope < other.scope));
}

bool Occurrence::hasSameScopeSizeAs(Occurrence const &other) const
{
   return scope.size() == other.scope.size();
}

void Occurrence::setAppearance(Style style)
{
   appearance = std::move(style);
}

Style Occurrence::getAppearance() const
{
   return appearance;
}

void Occurrence::setType(Identifier typeTopicId)
{
   type = typeTopicId;
}

void Occurrence::clearType()
{
   type.clear();
}

OptionalIdentifier Occurrence::getType() const
{
   return type;
}

void Occurrence::moveBy(contomap::model::SpacialCoordinate::Offset offset)
{
   location.moveBy(offset);
}
