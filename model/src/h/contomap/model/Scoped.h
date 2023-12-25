#pragma once

#include "contomap/infrastructure/LinkedReferences.h"
#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/Identifiers.h"

namespace contomap::model
{

class Topic;

/**
 * A Scoped entity contains a list of topics that defines where that entity exists (is relevant).
 */
class Scoped
{
public:
   /**
    * Default constructor.
    */
   Scoped() = default;
   /**
    * Constructor.
    *
    * @param topicIds the identifiers this scope is about.
    */
   explicit Scoped(contomap::model::Identifiers topicIds);

   /**
    * Serializes the scope.
    *
    * @param coder the encoder to use.
    */
   void encodeScoped(contomap::infrastructure::serial::Encoder &coder) const;
   /**
    * Deserializes the scope.
    *
    * @param coder the decoder to use.
    * @param resolver the function to resolve the instances of topics.
    */
   void decodeScoped(contomap::infrastructure::serial::Decoder &coder, std::function<contomap::model::Topic &(contomap::model::Identifier)> const &resolver);

   /**
    * Return true if this instance is in the given scope.
    *
    * @param thatScope the scope to look for.
    * @return true if the instance is in given scope.
    */
   [[nodiscard]] bool isIn(Identifiers const &thatScope) const;

   /**
    * Return true if the scope of this instance contains given topic.
    *
    * @param topic the topic to check on.
    * @return true if the scope contains given topic.
    */
   [[nodiscard]] bool scopeContains(contomap::model::Topic const &topic) const;

   /**
    * Return true if the scope equals the given one.
    *
    * @param thatScope the scope to test for.
    * @return true in case the two scope values are equal.
    */
   [[nodiscard]] bool scopeEquals(contomap::model::Identifiers const &thatScope) const;

   /**
    * Use this method to sort scoped entities according to scope.
    *
    * @param other the other scoped to compare to.
    * @return true if this instance has a narrower (longer) scope than the other. Also returns true if same size but smaller ID values.
    */
   [[nodiscard]] bool hasNarrowerScopeThan(Scoped const &other) const;
   /**
    * Use this method to determine whether a scoped entity is equivalent when considering scopes.
    *
    * @param other the other scoped to compare to.
    * @return true if this and the other entities have the same size of their scopes.
    */
   [[nodiscard]] bool hasSameScopeSizeAs(Scoped const &other) const;

private:
   contomap::model::Identifiers topicIds;
};

}
