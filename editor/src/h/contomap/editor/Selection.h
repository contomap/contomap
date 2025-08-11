#pragma once

#include <map>
#include <variant>

#include "contomap/editor/SelectedType.h"
#include "contomap/infrastructure/Generator.h"
#include "contomap/infrastructure/LinkedReferences.h"
#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/Association.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/Occurrence.h"
#include "contomap/model/Role.h"

namespace contomap::editor
{

/**
 * Selection keeps track of items that are currently highlighted.
 */
class Selection
{
public:
   /** Base type to refer to for template arguments. */
   template <typename T> using BaseType = typename std::remove_cv<T>::type;

   /**
    * Default constructor.
    */
   Selection();

   /**
    * Deserialize a selection.
    *
    * @param coder the coder to use.
    * @param version the version to consider.
    * @param occurrenceResolver function to return a known occurrence.
    * @param associationResolver function to return a known association.
    * @param roleResolver function to return a known role.
    * @return the decoded selection instance.
    */
   [[nodiscard]] static Selection from(contomap::infrastructure::serial::Decoder &coder, uint8_t version,
      std::function<contomap::model::Occurrence &(contomap::model::Identifier)> const &occurrenceResolver,
      std::function<contomap::model::Association &(contomap::model::Identifier)> const &associationResolver,
      std::function<contomap::model::Role &(contomap::model::Identifier)> const &roleResolver);

   /**
    * Serializes the selection information.
    *
    * @param coder the coder to use.
    */
   void encode(contomap::infrastructure::serial::Encoder &coder) const;

   /**
    * @return true if currently nothing is selected.
    */
   [[nodiscard]] bool empty() const;

   /**
    * Empties the selection.
    */
   void clear();

   /**
    * @return true if the selection has only one item selected.
    */
   [[nodiscard]] bool hasSoleEntry() const;

   /**
    * Determines whether the selection contains only a single entry of given type.
    * This is useful for entry-specific operations.
    *
    * @param type the queried type.
    * @return true if the selection has only one item selected, and it is of given type.
    */
   [[nodiscard]] bool hasSoleEntryFor(contomap::editor::SelectedType type) const;

   /**
    * Set the sole selection to be for the referenced type.
    *
    * @tparam T the type of the thing.
    * @param entry the thing to select.
    */
   template <class T> void setSole(BaseType<T> &entry)
   {
      clear();
      listFor<BaseType<T>>().add(entry);
   }

   /**
    * Toggles the selection of the referenced thing.
    *
    * @tparam T the type of the thing.
    * @param entry the thing to toggle.
    */
   template <class T> void toggle(BaseType<T> &entry)
   {
      auto &list = listFor<BaseType<T>>();
      if (list.contains(entry))
      {
         list.remove(entry);
      }
      else
      {
         list.add(entry);
      }
   }

   /**
    * Determines whether a specific thing is part of the selection.
    *
    * @tparam T the type of the thing to check.
    * @param entry the reference to the thing to check.
    * @return true if the referenced thing is in the selection.
    */
   template <class T> [[nodiscard]] bool contains(T const &entry) const
   {
      return listFor<BaseType<T>>().contains(entry);
   }

   /**
    * Provide the set of selected identifiers for given type.
    *
    * @param type the type to filter for.
    * @return a set of identifiers for the given selected type.
    */
   [[nodiscard]] contomap::model::Identifiers of(contomap::editor::SelectedType type) const;

   /**
    * @return a search of all occurrences in the selection.
    */
   template <class T> [[nodiscard]] contomap::infrastructure::Search<T> of() const
   {
      return listFor<T>().allReferences();
   }

private:
   using SelectionList = std::variant<contomap::infrastructure::LinkedReferences<contomap::model::Occurrence>,
      contomap::infrastructure::LinkedReferences<contomap::model::Association>, contomap::infrastructure::LinkedReferences<contomap::model::Role>>;

   static constexpr size_t SELECTION_TYPES_COUNT = std::variant_size_v<SelectionList>;

   using SelectionLists = std::array<SelectionList, SELECTION_TYPES_COUNT>;

   template <class T> struct Marker
   {
      static contomap::editor::SelectedType const TYPE;
   };

   template <class T> static constexpr contomap::editor::SelectedType typeOf()
   {
      return Marker<typename std::remove_const<T>::type>::TYPE;
   }

   template <class T> [[nodiscard]] contomap::infrastructure::LinkedReferences<T> &listFor()
   {
      return std::get<contomap::infrastructure::LinkedReferences<T>>(lists.at(static_cast<size_t>(typeOf<T>())));
   }

   template <class T> [[nodiscard]] contomap::infrastructure::LinkedReferences<T> const &listFor() const
   {
      return std::get<contomap::infrastructure::LinkedReferences<T>>(lists.at(static_cast<size_t>(typeOf<T>())));
   }

   [[nodiscard]] static contomap::infrastructure::References const &asReferences(SelectionList const &references);
   [[nodiscard]] static contomap::infrastructure::References &asReferences(SelectionList &references);

   template <class T> [[nodiscard]] static std::function<contomap::model::Identifiers(SelectionLists const &)> identifiersFor()
   {
      return [](SelectionLists const &selectionLists) {
         contomap::model::Identifiers ids;
         auto const &references = std::get<contomap::infrastructure::LinkedReferences<T>>(selectionLists.at(static_cast<size_t>(typeOf<T>())));
         for (contomap::model::Identifiable const &reference : references.allReferences())
         {
            ids.add(reference.getId());
         }
         return ids;
      };
   }

   template <class T> void encodeList(contomap::infrastructure::serial::Encoder &coder, std::string const &name) const
   {
      auto &list = listFor<T>();
      auto view = std::views::common(list.allReferences());
      coder.codeArray(name, view.begin(), view.end(), [](contomap::infrastructure::serial::Encoder &nested, T const &ref) { ref.getId().encode(nested, ""); });
   }

   template <class T>
   void decodeList(contomap::infrastructure::serial::Decoder &coder, std::string const &name, std::function<T &(contomap::model::Identifier)> const &resolver)
   {
      auto &list = listFor<T>();
      coder.codeArray(name,
         [&list, &resolver](contomap::infrastructure::serial::Decoder &nested, size_t) { list.add(resolver(contomap::model::Identifier::from(nested, ""))); });
   }

   SelectionLists lists;
};

} // namespace contomap::frontend
