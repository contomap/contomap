#pragma once

#include <memory>

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/model/Identifier.h"

namespace contomap::frontend
{
/**
 * A FocusItem is an adapter for concrete focused items of a map.
 */
class FocusItem
{
public:
   virtual ~FocusItem() = default;

   /**
    * Tests whether the item is an association with the given identifier.
    *
    * @param otherId the identifier to test against.
    * @return true if this item is the identified association.
    */
   [[nodiscard]] virtual bool isAssociation([[maybe_unused]] contomap::model::Identifier otherId) const
   {
      return false;
   }

   /**
    * Tests whether the item is an role with the given identifier.
    *
    * @param otherId the identifier to test against.
    * @return true if this item is the identified role.
    */
   [[nodiscard]] virtual bool isRole([[maybe_unused]] contomap::model::Identifier otherId) const
   {
      return false;
   }

   /**
    * Tests whether the item is an occurrence with the given identifier.
    *
    * @param otherId the identifier to test against.
    * @return true if this item is the identified occurrence.
    */
   [[nodiscard]] virtual bool isOccurrence([[maybe_unused]] contomap::model::Identifier otherId) const
   {
      return false;
   }

   /**
    * Modify the selection on the input request handler with given action.
    *
    * @param handler the handler to call.
    * @param action the action to forward.
    */
   virtual void modifySelection(contomap::editor::InputRequestHandler &handler, contomap::editor::SelectionAction action) const = 0;
};

/**
 * An AssociationFocusItem represents an association.
 */
class AssociationFocusItem : public FocusItem
{
public:
   /**
    * Constructor.
    *
    * @param id the identifier of the association.
    */
   explicit AssociationFocusItem(contomap::model::Identifier id)
      : id(id)
   {
   }

   /**
    * Tests whether the item is an association with the given identifier.
    *
    * @param otherId the identifier to test against.
    * @return true if this item is the identified association.
    */
   [[nodiscard]] bool isAssociation(contomap::model::Identifier otherId) const override
   {
      return id == otherId;
   }

   void modifySelection(contomap::editor::InputRequestHandler &handler, contomap::editor::SelectionAction action) const override
   {
      handler.modifySelection(contomap::editor::SelectedType::Association, id, action);
   }

private:
   contomap::model::Identifier id;
};

/**
 * A RoleFocusItem represents a role.
 */
class RoleFocusItem : public FocusItem
{
public:
   /**
    * Constructor.
    *
    * @param id the identifier of the role.
    */
   explicit RoleFocusItem(contomap::model::Identifier id)
      : id(id)
   {
   }

   /**
    * Tests whether the item is an role with the given identifier.
    *
    * @param otherId the identifier to test against.
    * @return true if this item is the identified role.
    */
   [[nodiscard]] bool isRole(contomap::model::Identifier otherId) const override
   {
      return id == otherId;
   }

   void modifySelection(contomap::editor::InputRequestHandler &handler, contomap::editor::SelectionAction action) const override
   {
      handler.modifySelection(contomap::editor::SelectedType::Role, id, action);
   }

private:
   contomap::model::Identifier id;
};

/**
 * An OccurrenceFocusItem represents an occurrence.
 */
class OccurrenceFocusItem : public FocusItem
{
public:
   /**
    * Constructor.
    *
    * @param id the identifier of the occurrence.
    */
   explicit OccurrenceFocusItem(contomap::model::Identifier id)
      : id(id)
   {
   }

   /**
    * Tests whether the item is an occurrence with the given identifier.
    *
    * @param otherId the identifier to test against.
    * @return true if this item is the identified occurrence.
    */
   [[nodiscard]] bool isOccurrence(contomap::model::Identifier otherId) const override
   {
      return id == otherId;
   }

   void modifySelection(contomap::editor::InputRequestHandler &handler, contomap::editor::SelectionAction action) const override
   {
      handler.modifySelection(contomap::editor::SelectedType::Occurrence, id, action);
   }

private:
   contomap::model::Identifier id;
};

/**
 * Focus is a container that accumulates focused items and determines which one is the closests to match.
 */
class Focus
{
public:
   /**
    * Constructor.
    */
   Focus();

   /**
    * @return true if currently nothing is focused.
    */
   [[nodiscard]] bool hasNoItem() const
   {
      return item == nullptr;
   }

   /**
    * Registers a new item with given distance. Should the distance be shorter than any previously registered item,
    * then this new item receives focus.
    *
    * @param newItem the new item to focus.
    * @param newDistance the distance to consider for this item.
    */
   void registerItem(std::shared_ptr<FocusItem> newItem, float newDistance);

   /**
    * Tests whether the focused item is an association with the given identifier.
    *
    * @param otherId the identifier to test against.
    * @return true if this item is the identified association.
    */
   [[nodiscard]] bool isAssociation(contomap::model::Identifier otherId) const
   {
      return (item != nullptr) && item->isAssociation(otherId);
   }

   /**
    * Tests whether the focused item is an role with the given identifier.
    *
    * @param otherId the identifier to test against.
    * @return true if this item is the identified role.
    */
   [[nodiscard]] bool isRole(contomap::model::Identifier otherId) const
   {
      return (item != nullptr) && item->isRole(otherId);
   }

   /**
    * Tests whether the item is an occurrence with the given identifier.
    *
    * @param otherId the identifier to test against.
    * @return true if this item is the identified occurrence.
    */
   [[nodiscard]] virtual bool isOccurrence(contomap::model::Identifier otherId) const
   {
      return (item != nullptr) && item->isOccurrence(otherId);
   }

   /**
    * Modify the selection on the input request handler with given action.
    *
    * @param handler the handler to call.
    * @param action the action to forward.
    */
   void modifySelection(contomap::editor::InputRequestHandler &handler, contomap::editor::SelectionAction action) const;

private:
   std::shared_ptr<FocusItem> item;
   float distance;
};

}
