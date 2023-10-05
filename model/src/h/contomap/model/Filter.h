#pragma once

#include <functional>
#include <memory>

#include "contomap/model/Topic.h"

namespace contomap::model
{

class ContomapView;

/**
 * Filter is used to query things with specific properties.
 */
template <class FilteredType> class Filter
{
public:
   /** Function is a functional representation of a filter. */
   using Function = std::function<bool(FilteredType const &, contomap::model::ContomapView const &)>;

   virtual ~Filter() = default;

   /**
    * Factory function for creating a Filter that is based on a function.
    * Use this variant to create a filter in case the filter can be expressed as a lambda.
    *
    * @param fn the function to call for each item.
    * @return a Filter based on provided function.
    */
   [[nodiscard]] static std::unique_ptr<Filter<FilteredType>> of(Function fn)
   {
      return std::make_unique<FunctionFilter<FilteredType>>(std::move(fn));
   }

   /**
    * Test whether a specific instance is passing the filter.
    *
    * @param instance the thing to test.
    * @param view the view within to verify indirect properties.
    * @return true in case the topic matches (passes) the filter, false otherwise.
    */
   [[nodiscard]] virtual bool matches(FilteredType const &instance, contomap::model::ContomapView const &view) const = 0;

private:
   template <class Type> class FunctionFilter : public Filter<Type>
   {
   public:
      /**
       * Constructor.
       *
       * @param fn the function to wrap.
       */
      explicit FunctionFilter(Filter<Type>::Function fn)
         : fn(std::move(fn))
      {
      }

      [[nodiscard]] bool matches(Type const &instance, contomap::model::ContomapView const &view) const override
      {
         return fn(instance, view);
      }

   private:
      Filter<Type>::Function fn;
   };
};

}
