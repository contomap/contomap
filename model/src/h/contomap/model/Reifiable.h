#pragma once

#include <optional>

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
    * @return the refined reifier instance that describes this reifiable, if set.
    */
   [[nodiscard]] std::optional<std::reference_wrapper<T>> getReifier() const
   {
      if (hasNoReifier())
      {
         return {};
      }
      return { reifier->refine() };
   }

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
