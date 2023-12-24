#pragma once

#include "contomap/infrastructure/Link.h"
#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/Topic.h"

namespace contomap::model
{

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
   void setType(contomap::model::Topic &topic)
   {
      clearType();
      type = topic.linkTyped([this]() { type.reset(); });
   }

   /**
    * @return true if this typeable has a type associated.
    */
   [[nodiscard]] bool hasType() const
   {
      return !hasNoType();
   }

   /**
    * @return the topic instance that classifies this typeable, if set.
    */
   [[nodiscard]] std::optional<std::reference_wrapper<Topic>> getType()
   {
      if (hasNoType())
      {
         return {};
      }
      return { type->getLinked() };
   }

   /**
    * @return the topic instance that classifies this typeable, if set.
    */
   [[nodiscard]] std::optional<std::reference_wrapper<Topic const>> getType() const
   {
      if (hasNoType())
      {
         return {};
      }
      return { type->getLinked() };
   }

   /**
    * Removes any associated type.
    */
   void clearType()
   {
      if (hasNoType())
      {
         return;
      }
      auto old = std::move(type);
      old.reset();
   }

   /**
    * Serializes the reference of the type.
    *
    * @param coder the encoder to use.
    */
   void encodeTypeable(contomap::infrastructure::serial::Encoder &coder) const
   {
      contomap::infrastructure::serial::Coder::Scope scope(coder, "type");
      uint8_t marker = hasType() ? 0x01 : 0x00;
      coder.code("present", marker);
      if (marker != 0x00)
      {
         type->getLinked().getId().encode(coder, "id");
      }
   }

   /**
    * Deserializes the reference to a type.
    *
    * @param coder the decoder to use.
    * @param resolver the function to resolve the instance of the referenced topic.
    */
   void decodeTypeable(contomap::infrastructure::serial::Decoder &coder, std::function<Topic &(contomap::model::Identifier)> const &resolver)
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

protected:
   ~Typeable()
   {
      clearType();
   }

private:
   [[nodiscard]] bool hasNoType() const
   {
      return type == nullptr;
   }

   std::unique_ptr<contomap::infrastructure::Link<contomap::model::Topic>> type;
};

}
