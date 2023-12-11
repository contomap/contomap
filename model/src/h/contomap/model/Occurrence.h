#pragma once

#include <memory>

#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/Coordinates.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/OptionalIdentifier.h"
#include "contomap/model/Reifiable.h"
#include "contomap/model/Style.h"

namespace contomap::model
{

class Topic;

/**
 * An Occurrence represents the presence of a topic.
 */
class Occurrence : public contomap::model::Reifiable<contomap::model::Topic>
{
public:
   /**
    * Constructor.
    *
    * @param id the primary identifier of this occurrence.
    * @param topic the reference to the topic this occurrence represents.
    * @param scope the scope within which this occurrence is valid.
    * @param spacial the known, initial point where the occurrence is happening.
    */
   Occurrence(contomap::model::Identifier id, contomap::model::Topic &topic, contomap::model::Identifiers scope, contomap::model::SpacialCoordinate spacial);

   /**
    * Deserializes the occurrence.
    *
    * @param coder the coder to use.
    * @param version the version to consider.
    * @param id the primary identifier of this occurrence.
    * @param topic the topic this occurrence represents.
    * @param topicResolver the function to use for resolving topic references.
    * @return the decoded instance.
    */
   [[nodiscard]] static std::unique_ptr<Occurrence> from(contomap::infrastructure::serial::Decoder &coder, uint8_t version, contomap::model::Identifier id,
      Topic &topic, std::function<Topic &(contomap::model::Identifier)> const &topicResolver);

   /**
    * Serializes the occurrence.
    *
    * @param coder the coder to use.
    */
   void encode(contomap::infrastructure::serial::Encoder &coder) const;

   /**
    * @return the unique identifier of this occurrence instance.
    */
   [[nodiscard]] contomap::model::Identifier getId() const;

   /**
    * @return the topic this occurrence represents.
    */
   [[nodiscard]] contomap::model::Topic const &getTopic() const;

   /**
    * @return the scope of this occurrence.
    */
   [[nodiscard]] contomap::model::Identifiers const &getScope() const;

   /**
    * @return the location of this occurrence
    */
   [[nodiscard]] contomap::model::Coordinates const &getLocation() const;

   /**
    * Return true if this instance is in the given scope.
    *
    * @param thatScope the scope to look for.
    * @return true if the occurrence is in given scope.
    */
   [[nodiscard]] bool isIn(contomap::model::Identifiers const &thatScope) const;

   /**
    * Return true if the scope contains the given identifier.
    *
    * @param thatId the identifier to check .
    * @return true in case the identifier is part of the scope.
    */
   [[nodiscard]] bool scopeContains(contomap::model::Identifier thatId) const;

   /**
    * Use this method to sort occurrences according to scope.
    *
    * @param other the other occurrence to compare to.
    * @return true if this occurrence has a narrower (longer) scope than the other. Also returns true if same size but smaller ID values.
    */
   [[nodiscard]] bool hasNarrowerScopeThan(Occurrence const &other) const;
   /**
    * Use this method to determine whether an occurrence is equivalent when considering scopes.
    *
    * @param other the other occurrence to compare to.
    * @return true if this and the other occurrence have the same size of their scopes.
    */
   [[nodiscard]] bool hasSameScopeSizeAs(Occurrence const &other) const;

   /**
    * Set the style of the appearance.
    *
    * @param style the new style to set.
    */
   void setAppearance(contomap::model::Style style);
   /**
    * @return the current style of the appearance.
    */
   [[nodiscard]] contomap::model::Style getAppearance() const;

   /**
    * Assign the type of this occurrence.
    *
    * @param typeTopicId the identifier of the topic that describes this occurrence.
    */
   void setType(contomap::model::Identifier typeTopicId);
   /**
    * Clears the type of this occurrence.
    */
   void clearType();
   /**
    * @return the identifier of the topic that describes this occurrence, if set.
    */
   [[nodiscard]] contomap::model::OptionalIdentifier getType() const;

private:
   Occurrence(contomap::model::Identifier id, contomap::model::Topic &topic);

   contomap::model::Identifier id;
   contomap::model::Topic &topic;
   contomap::model::Identifiers scope;

   contomap::model::Coordinates location;

   contomap::model::OptionalIdentifier type;
   contomap::model::Style appearance;
};

}
