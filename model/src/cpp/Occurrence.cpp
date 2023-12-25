#include "contomap/model/Occurrence.h"
#include "contomap/model/Topic.h"

using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::Style;
using contomap::model::Topic;

Occurrence::Occurrence(Identifier id, Topic &topic, Identifiers scope, SpacialCoordinate spacial)
   : Scoped(std::move(scope))
   , id(id)
   , topic(topic)
   , location(spacial)
{
}

Occurrence::Occurrence(Identifier id, Topic &topic)
   : id(id)
   , topic(topic)
{
}

Occurrence &Occurrence::refine()
{
   return *this;
}

std::unique_ptr<Occurrence> Occurrence::from(contomap::infrastructure::serial::Decoder &coder, uint8_t version, contomap::model::Identifier id, Topic &topic,
   std::function<Topic &(contomap::model::Identifier)> const &topicResolver)
{
   Coder::Scope serialScope(coder, "occurrence");
   std::unique_ptr<Occurrence> occurrence(new Occurrence(id, topic));
   occurrence->decodeScoped(coder, topicResolver);
   occurrence->location.decode(coder, "location", version);
   occurrence->decodeTypeable(coder, topicResolver);
   occurrence->appearance.decode(coder, "appearance", version);
   occurrence->decodeReifiable(coder, topicResolver);
   return occurrence;
}

void Occurrence::encode(Encoder &coder) const
{
   Coder::Scope serialScope(coder, "occurrence");
   encodeScoped(coder);
   location.encode(coder, "location");
   encodeTypeable(coder);
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

contomap::model::Coordinates const &Occurrence::getLocation() const
{
   return location;
}

void Occurrence::setAppearance(Style style)
{
   appearance = std::move(style);
}

Style Occurrence::getAppearance() const
{
   return appearance;
}

void Occurrence::moveBy(contomap::model::SpacialCoordinate::Offset offset)
{
   location.moveBy(offset);
}
