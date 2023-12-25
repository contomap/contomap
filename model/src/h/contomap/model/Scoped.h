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

private:
   contomap::model::Identifiers topicIds;
};

}
