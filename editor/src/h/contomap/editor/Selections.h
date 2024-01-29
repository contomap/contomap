#pragma once

#include "contomap/editor/Selection.h"
#include "contomap/infrastructure/Generator.h"
#include "contomap/infrastructure/Traits.h"
#include "contomap/model/Contomap.h"
#include "contomap/model/ContomapView.h"
#include "contomap/model/Style.h"

namespace contomap::editor
{

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
    * @return the reference to the found occurrence - if selected and existing.
    */
   template <class SelectionType>
   [[nodiscard]] static std::optional<
      std::reference_wrapper<typename contomap::infrastructure::constness_of<SelectionType>::template applied_to_t<contomap::model::Occurrence>>>
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
    * @return the reference to the found topic - if selected and existing.
    */
   [[nodiscard]] static std::optional<std::reference_wrapper<contomap::model::Topic const>> topicOfFirstOccurrenceFrom(
      contomap::editor::Selection const &selection);

   /**
    * Determines the first appearance of any selected item.
    *
    * @param selection the selection containing selection identifier(s).
    * @return the resulting style.
    */
   [[nodiscard]] static std::optional<contomap::model::Style> firstAppearanceFrom(contomap::editor::Selection const &selection);

   /**
    * Retrieves the first reifiable from the selection.
    *
    * @param selection the selection containing selection identifier(s).
    * @return the reference to the found reifiable - if selected and existing.
    */
   [[nodiscard]] static std::optional<std::reference_wrapper<contomap::model::Reifiable<contomap::model::Topic> const>> firstReifiableFrom(
      contomap::editor::Selection const &selection);

   /**
    * Provide a view on all the typeables from the selection.
    *
    * @param selection the selection to extract.
    * @return a search that provides all selected typeables.
    */
   [[nodiscard]] static contomap::infrastructure::Search<contomap::model::Typeable> allTypeableFrom(contomap::editor::Selection const &selection);
   /**
    * Provide a view on all the reifiables from the selection.
    *
    * @param selection the selection to extract.
    * @return a search that provides all selected reifiables.
    */
   [[nodiscard]] static contomap::infrastructure::Search<contomap::model::Reifiable<contomap::model::Topic>> allReifiableFrom(
      contomap::editor::Selection const &selection);
   /**
    * Provide a view on all the styleables from the selection.
    *
    * @param selection the selection to extract.
    * @return a search that provides all selected styleables.
    */
   [[nodiscard]] static contomap::infrastructure::Search<contomap::model::Styleable> allStyleableFrom(contomap::editor::Selection const &selection);

private:
   template <typename R, typename Base> static contomap::infrastructure::Search<R> allFrom(contomap::editor::Selection const &, std::function<R &(Base &)>)
   {
      return {};
   }

   template <typename R, typename Base, typename Type, typename... Types>
   static contomap::infrastructure::Search<R> allFrom(contomap::editor::Selection const &selection, std::function<R &(Base &)> toR)
   {
      for (auto &entry : selection.of<Type>())
      {
         co_yield toR(entry);
      }
      for (R &r : allFrom<R, Base, Types...>(selection, toR))
      {
         co_yield r;
      }
   }
};

} // namespace contomap::editor
