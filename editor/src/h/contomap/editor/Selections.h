#pragma once

#include <type_traits>

#include "contomap/editor/Selection.h"
#include "contomap/infrastructure/Generator.h"
#include "contomap/model/Contomap.h"
#include "contomap/model/ContomapView.h"
#include "contomap/model/Style.h"

namespace contomap::editor
{

// from https://codereview.stackexchange.com/questions/208121/add-const-or-remove-const-based-on-another-type
template <typename U> struct constness_of
{
   enum
   {
      value = std::is_const<U>::value
   };
   template <typename T> struct applied_to
   {
      using type = typename std::conditional<value, typename std::add_const<T>::type, typename std::remove_const<T>::type>::type;
   };

   template <typename T> using applied_to_t = typename applied_to<T>::type;
};

/**
 * Selections provides helper functions for resolving items.
 */
class Selections
{
public:
   /**
    * Retrieves the first occurrence from the selection.
    *
    * @param selection the selection containing selection identifier(s).
    * @param view the view through which to resolve the objects.
    * @return the reference to the found occurrence - if selected and existing.
    */
   [[nodiscard]] static std::optional<std::reference_wrapper<contomap::model::Occurrence const>> firstOccurrenceFrom(
      contomap::editor::Selection const &selection, contomap::model::ContomapView const &view);

   template <class SelectionType>
   [[nodiscard]] static std::optional<std::reference_wrapper<typename constness_of<SelectionType>::template applied_to_t<contomap::model::Occurrence>>>
   firstOccurrenceFrom(SelectionType &selection)
   {
      for (auto &occurrence : selection.template of<contomap::model::Occurrence>())
      {
         return { occurrence };
      }
      return {};
   }

   /**
    * Retrieves the first topic from the selection of occurrences.
    *
    * @param selection the selection containing selection identifier(s).
    * @param view the view through which to resolve the objects.
    * @return the reference to the found topic - if selected and existing.
    */
   [[nodiscard]] static std::optional<std::reference_wrapper<contomap::model::Topic const>> topicOfFirstOccurrenceFrom(
      contomap::editor::Selection const &selection, contomap::model::ContomapView const &view);

   /**
    * Determines the first appearance of any selected item.
    *
    * @param selection the selection containing selection identifier(s).
    * @param view the view through which to resolve the objects.
    * @return the resulting style.
    */
   [[nodiscard]] static std::optional<contomap::model::Style> firstAppearanceFrom(
      contomap::editor::Selection const &selection, contomap::model::ContomapView const &view);

   /**
    * Retrieves the first reifiable from the selection.
    *
    * @param selection the selection containing selection identifier(s).
    * @param view the view through which to resolve the objects.
    * @return the reference to the found reifiable - if selected and existing.
    */
   [[nodiscard]] static std::optional<std::reference_wrapper<contomap::model::Reifiable<contomap::model::Topic> const>> firstReifiableFrom(
      contomap::editor::Selection const &selection, contomap::model::ContomapView const &view);

   /**
    * Provide a view on all the typeables from the selection.
    *
    * @param selection the selection to extract.
    * @param map the map to use for transformation.
    * @return a search that provides all selected typeables.
    */
   [[nodiscard]] static contomap::infrastructure::Search<contomap::model::Typeable> allTypeableFrom(
      contomap::editor::Selection const &selection, contomap::model::Contomap &map);
   /**
    * Provide a view on all the reifiables from the selection.
    *
    * @param selection the selection to extract.
    * @param map the map to use for transformation.
    * @return a search that provides all selected reifiables.
    */
   [[nodiscard]] static contomap::infrastructure::Search<contomap::model::Reifiable<contomap::model::Topic>> allReifiableFrom(
      contomap::editor::Selection const &selection, contomap::model::Contomap &map);
   /**
    * Provide a view on all the styleables from the selection.
    *
    * @param selection the selection to extract.
    * @param map the map to use for transformation.
    * @return a search that provides all selected styleables.
    */
   [[nodiscard]] static contomap::infrastructure::Search<contomap::model::Styleable> allStyleableFrom(
      contomap::editor::Selection const &selection, contomap::model::Contomap &map);

   /**
    * Provide a search over the occurrence references from the selection.
    *
    * @param selection the selection to extract.
    * @param map the map to use for transformation.
    * @return a search that provides all occurrences.
    */
   [[nodiscard]] static contomap::infrastructure::Search<contomap::model::Occurrence> selectedOccurrences(
      contomap::editor::Selection const &selection, contomap::model::Contomap &map);
   /**
    * Provide a search over the association references from the selection.
    *
    * @param selection the selection to extract.
    * @param map the map to use for transformation.
    * @return a search that provides all associations.
    */
   [[nodiscard]] static contomap::infrastructure::Search<contomap::model::Association> selectedAssociations(
      contomap::editor::Selection const &selection, contomap::model::Contomap &map);
   /**
    * Provide a search over the role references from the selection.
    *
    * @param selection the selection to extract.
    * @param map the map to use for transformation.
    * @return a search that provides all roles.
    */
   [[nodiscard]] static contomap::infrastructure::Search<contomap::model::Role> selectedRoles(
      contomap::editor::Selection const &selection, contomap::model::Contomap &map);
};

} // namespace contomap::editor
