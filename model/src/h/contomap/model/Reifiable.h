#pragma once

#include <optional>

#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/Reified.h"
#include "contomap/model/Reifier.h"

namespace contomap::model
{

/**
 * A Reifiable is something that can be described by a reifier instance.
 */
template <class T> class Reifiable : public contomap::model::Reified
{
public:
   /**
    * Assign the reifier of this reifiable.
    * If the reifiable is already reified, it will disconnect the old one before.
    *
    * @param newReifier the reference to the reifying instance.
    */
   void setReifier(contomap::model::Reifier<T> &newReifier)
   {
      clearReifier();
      reifier = &newReifier;
      newReifier.setReified(*this);
   }

   /**
    * @return true if this reifiable has a reifier associated.
    */
   [[nodiscard]] bool hasReifier() const
   {
      return !hasNoReifier();
   }

   /**
    * @return the refined reifier instance that describes this reifiable, if set.
    */
   [[nodiscard]] std::optional<std::reference_wrapper<T>> getReifier()
   {
      if (hasNoReifier())
      {
         return {};
      }
      return { reifier->refine() };
   }

   /**
    * @return the refined reifier instance that describes this reifiable, if set.
    */
   [[nodiscard]] std::optional<std::reference_wrapper<T const>> getReifier() const
   {
      if (hasNoReifier())
      {
         return {};
      }
      return { reifier->refine() };
   }

   /**
    * @copydoc contomap::model::Reified::clearReifier()
    */
   void clearReifier() final
   {
      if (hasNoReifier())
      {
         return;
      }
      auto &old = *reifier;
      reifier = nullptr;
      old.clearReified();
   }

   /**
    * Serializes the reference of the reifier.
    *
    * @param coder the coder to use.
    */
   void encodeReifiable(contomap::infrastructure::serial::Encoder &coder) const
   {
      uint8_t marker = hasReifier() ? 0x01 : 0x00;
      coder.code("hasReifier", marker);
      if (marker != 0x00)
      {
         reifier->getId().encode(coder, "reifier");
      }
   }

   /**
    * Deserializes the reference to a reifier.
    *
    * @param coder the coder to use.
    * @param resolver the function to resolve the instance of the referenced reifier.
    */
   void decodeReifiable(contomap::infrastructure::serial::Decoder &coder, std::function<Reifier<T> &(contomap::model::Identifier)> const &resolver)
   {
      uint8_t marker = 0x00;
      coder.code("hasReifier", marker);
      if (marker != 0x00)
      {
         auto reifierId = contomap::model::Identifier::from(coder, "reifier");
         setReifier(resolver(reifierId));
      }
   }

protected:
   ~Reifiable() override
   {
      clearReifier();
   }

private:
   [[nodiscard]] bool hasNoReifier() const
   {
      return reifier == nullptr;
   }

   contomap::model::Reifier<T> *reifier = nullptr;
};

}
