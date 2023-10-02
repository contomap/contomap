#pragma once

#include "contomap/editor/SelectedType.h"
#include "contomap/editor/SelectionAction.h"
#include "contomap/editor/SelectionMode.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/SpacialCoordinate.h"
#include "contomap/model/TopicNameValue.h"

namespace contomap::editor
{

/**
 * InputRequestHandler is the incoming interface from user requests.
 */
class InputRequestHandler
{
public:
   virtual ~InputRequestHandler() = default;

   /**
    * Called when a new topic shall be created.
    *
    * @param name how the new topic shall be named.
    * @param location the point at which the topic shall be put at.
    * @return identifier of the created topic
    */
   virtual contomap::model::Identifier newTopicRequested(contomap::model::TopicNameValue name, contomap::model::SpacialCoordinate location) = 0;

   /**
    * Called when a new association shall be created.
    *
    * @param location the point at which the association shall be put at.
    */
   virtual contomap::model::Identifier newAssociationRequested(contomap::model::SpacialCoordinate location) = 0;

   /**
    * Called to clear the current selection.
    */
   virtual void clearSelection() = 0;

   /**
    * Called to modify the active selection.
    *
    * @param type the type of the identified thing.
    * @param id the unique identifier of the focused item.
    * @param action what kind of selection action is requested.
    * @param mode how to perform the action.
    */
   virtual void modifySelection(
      contomap::editor::SelectedType type, contomap::model::Identifier id, contomap::editor::SelectionAction action, contomap::editor::SelectionMode mode)
      = 0;

   /**
    * Called to request a link between the selected items.
    * In case the selection contains one (or more) associations, all the topics from the selected occurrences will be assigned to them.
    * In case the selection contains only occurrences, their respective topics will be linked with one new association.
    */
   virtual void linkSelection() = 0;
};

} // namespace contomap::editor
