#pragma once

#include <type_traits>

namespace contomap::infrastructure
{

/**
 * constness_of allows to forward the const state of a source type to another one.
 * Use this helper to avoid duplicating access methods in both const/non-const variants.
 *
 * From https://codereview.stackexchange.com/questions/208121/add-const-or-remove-const-based-on-another-type
 *
 * @tparam U the source type to consider
 */
template <typename U> struct constness_of
{
   enum
   {
      value = std::is_const<U>::value
   };

   /**
    * applied_to sets the const state according to the source type
    *
    * @tparam T the type to apply to
    */
   template <typename T> struct applied_to
   {
      /**
       * The resulting type.
       */
      using type = typename std::conditional<value, typename std::add_const<T>::type, typename std::remove_const<T>::type>::type;
   };

   /**
    * Forwarder type.
    */
   template <typename T> using applied_to_t = typename applied_to<T>::type;
};

}