#pragma once

#include "contomap/infrastructure/Link.h"
#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"

namespace contomap::model
{

class Topic;

/**
 * A Typeable is something that can be classified by a topic instance.
 */
class Typeable
{
public:
   /**
    * Assign a topic as the new type.
    * If the typeable has a type, it will disconnect the old one before.
    *
    * @param topic the reference to the type.
    */
   void setType(contomap::model::Topic &topic);

   /**
    * @return true if this typeable has a type associated.
    */
   [[nodiscard]] bool hasType() const;

   /**
    * @return the topic instance that classifies this typeable, if set.
    */
   [[nodiscard]] std::optional<std::reference_wrapper<Topic>> getType();

   /**
    * @return the topic instance that classifies this typeable, if set.
    */
   [[nodiscard]] std::optional<std::reference_wrapper<Topic const>> getType() const;

   /**
    * Removes any associated type.
    */
   void clearType();

   /**
    * Serializes the reference of the type.
    *
    * @param coder the encoder to use.
    */
   void encodeTypeable(contomap::infrastructure::serial::Encoder &coder) const;

   /**
    * Deserializes the reference to a type.
    *
    * @param coder the decoder to use.
    * @param resolver the function to resolve the instance of the referenced topic.
    */
   void decodeTypeable(contomap::infrastructure::serial::Decoder &coder, std::function<contomap::model::Topic &(contomap::model::Identifier)> const &resolver);

protected:
   ~Typeable();

private:
   [[nodiscard]] bool hasNoType() const;

   std::unique_ptr<contomap::infrastructure::Link<contomap::model::Topic>> type;
};

}
