#pragma once

#include <map>

#include "contomap/editor/SelectedType.h"
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
   /**
    * Default constructor.
    */
   Selection() = default;

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
    * Set the sole selection to be for the identified type.
    *
    * @param type the type that the identifier refers to.
    * @param id identifier of the selected thing.
    */
   void setSole(contomap::editor::SelectedType type, contomap::model::Identifier id);

   /**
    * Set the sole selection to be for the referenced type.
    *
    * @param type the type that the identifier refers to.
    * @param id identifier of the selected thing.
    */
   template <class T> void setSole(T &entry)
   {
      setSole(typeOf<T>(), entry.getId());
   }

   /**
    * Toggles the selection of the identified thing.
    *
    * @param type the type that the identifier refers to.
    * @param id identifier for the selected thing.
    */
   void toggle(contomap::editor::SelectedType type, contomap::model::Identifier id);

   /**
    * Determines whether a specific thing is part of the selection.
    *
    * @param type the type that the identifier refers to.
    * @param id identifier for the occurrence to check.
    * @return true if the identified thing is in the selection.
    */
   [[nodiscard]] bool contains(contomap::editor::SelectedType type, contomap::model::Identifier id) const;

   /**
    * Determines whether a specific thing is part of the selection.
    *
    * @tparam T the type of the thing to check.
    * @param entry the reference to the thing to check.
    * @return true if the referenced thing is in the selection.
    */
   template <class T> [[nodiscard]] bool contains(T const &entry) const
   {
      return contains(typeOf<T>(), entry.getId());
   }

   /**
    * Provide the set of selected identifiers for given type.
    *
    * @param type the type to filter for.
    * @return a set of identifiers for the given selected type.
    */
   [[nodiscard]] contomap::model::Identifiers const &of(contomap::editor::SelectedType type) const;

private:
   template <class T> struct Marker
   {
      static contomap::editor::SelectedType const TYPE;
   };

   template <class T> static constexpr contomap::editor::SelectedType typeOf()
   {
      return Marker<typename std::remove_const<T>::type>::TYPE;
   }

   std::map<contomap::editor::SelectedType, contomap::model::Identifiers> identifiers;
};

} // namespace contomap::frontend
