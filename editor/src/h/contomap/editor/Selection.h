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
      // setSole(typeOf<T>(), entry.getId());
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
      // toggle(typeOf<T>(), entry.getId());
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
      // return contains(typeOf<T>(), entry.getId());
   }

   /**
    * Provide the set of selected identifiers for given type.
    *
    * @param type the type to filter for.
    * @return a set of identifiers for the given selected type.
    */
   [[nodiscard]] contomap::model::Identifiers of(contomap::editor::SelectedType type) const
   {
      contomap::model::Identifiers result;

      if (type == contomap::editor::SelectedType::Occurrence)
      {
         auto occurrences = of<contomap::model::Occurrence>();
         for (contomap::model::Occurrence const &occurrence : occurrences)
         {
            result.add(occurrence.getId());
         }
      }
      else if (type == contomap::editor::SelectedType::Association)
      {
         auto associations = of<contomap::model::Association>();
         for (contomap::model::Association const &association : associations)
         {
            result.add(association.getId());
         }
      }
      else if (type == contomap::editor::SelectedType::Role)
      {
         auto roles = of<contomap::model::Role>();
         for (contomap::model::Role const &role : roles)
         {
            result.add(role.getId());
         }
      }

      return result;
   }

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

   template <class T> struct Marker
   {
      static contomap::editor::SelectedType const TYPE;
   };

   template <class T> static constexpr contomap::editor::SelectedType typeOf()
   {
      return Marker<typename std::remove_const<T>::type>::TYPE;
   }

   template <class T> contomap::infrastructure::LinkedReferences<T> &listFor()
   {
      return std::get<contomap::infrastructure::LinkedReferences<T>>(lists.at(static_cast<size_t>(typeOf<T>())));
      /*
      auto it = lists.find(typeOf<T>());
      if (it == lists.end())
      {
         auto ptr = std::make_unique<SelectionList>(contomap::infrastructure::LinkedReferences<T> {});
         // it = lists.emplace(std::move(ptr)).first;
      }
      return std::get<contomap::infrastructure::LinkedReferences<T>>(*it->second);
      */
   }

   template <class T> contomap::infrastructure::LinkedReferences<T> const &listFor() const
   {
      return std::get<contomap::infrastructure::LinkedReferences<T>>(lists.at(static_cast<size_t>(typeOf<T>())));
      /*
      static contomap::infrastructure::LinkedReferences<T> const empty;
      auto it = lists.find(typeOf<T>());
      if (it == lists.end())
      {
         return empty;
      }
      return std::get<contomap::infrastructure::LinkedReferences<T>>(*it->second);
       */
   }

   [[nodiscard]] static contomap::infrastructure::Sized const &asSized(SelectionList const &list);
   [[nodiscard]] static contomap::infrastructure::Sized &asSized(SelectionList &list);

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

   /*
   void setSole(contomap::editor::SelectedType type, contomap::model::Identifier id);
   void toggle(contomap::editor::SelectedType type, contomap::model::Identifier id);
   [[nodiscard]] bool contains(contomap::editor::SelectedType type, contomap::model::Identifier id) const;
   */

   // std::map<contomap::editor::SelectedType, contomap::model::Identifiers> identifiers;
   // std::map<contomap::editor::SelectedType, std::unique_ptr<SelectionList>> lists;
   std::array<SelectionList, 3> lists;
};

} // namespace contomap::frontend
