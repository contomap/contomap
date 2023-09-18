#include "contomap/model/TopicFilter.h"

using contomap::model::TopicFilter;

/**
 * This class is a wrapper over a filter function.
 */
class FunctionFilter : public TopicFilter
{
public:
   /**
    * Constructor.
    *
    * @param fn the function to wrap.
    */
   explicit FunctionFilter(Function fn)
      : fn(std::move(fn))
   {
   }

   [[nodiscard]] bool matches(contomap::model::Topic const &topic, contomap::model::ContomapView const &view) const override
   {
      return fn(topic, view);
   }

private:
   Function fn;
};

std::unique_ptr<TopicFilter> TopicFilter::of(Function fn)
{
   return std::make_unique<FunctionFilter>(std::move(fn));
}
