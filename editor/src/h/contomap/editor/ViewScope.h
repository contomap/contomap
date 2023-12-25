#pragma once

#include "contomap/infrastructure/LinkedReferences.h"
#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/Topic.h"

namespace contomap::editor
{

/**
 * A ViewScope is a list of topics that define what is currently visible.
 */
class ViewScope
{
public:
   /**
    * Default constructor.
    */
   ViewScope() = default;
   /**
    * Constructor for a single topic.
    *
    * @param single the single topic to make a scope out of.
    */
   explicit ViewScope(contomap::model::Topic &single);

   /**
    * Deserializes a scope.
    *
    * @param coder the decoder to use.
    * @param resolver the function to resolve the instances of topics.
    */
   [[nodiscard]] static ViewScope from(
      contomap::infrastructure::serial::Decoder &coder, std::function<contomap::model::Topic &(contomap::model::Identifier)> const &resolver);

   /**
    * Serializes the scope.
    *
    * @param coder the encoder to use.
    */
   void encode(contomap::infrastructure::serial::Encoder &coder) const;

   /**
    * @return a scope list
    */
   [[nodiscard]] contomap::model::Identifiers identifiers() const;

   /**
    * @return true if the view scope is currently empty.
    */
   [[nodiscard]] bool empty() const;

   /**
    * Adds the given topic to the view scope.
    *
    * @param topic the topic to add.
    */
   void add(contomap::model::Topic &topic);
   /**
    * Removes the given topic from the view scope.
    *
    * @param topic the topic to remove.
    * @returns true if the given topic was removed.
    */
   bool remove(contomap::model::Topic &topic);

private:
   contomap::infrastructure::LinkedReferences<contomap::model::Topic> topics;
};

}
