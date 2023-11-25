#pragma once

#include "contomap/editor/SelectedType.h"
#include "contomap/editor/SelectionAction.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/SpacialCoordinate.h"
#include "contomap/model/Style.h"
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
    * @param name how the new topic shall be named by default.
    * @param location the point at which the first occurrence shall be put at.
    * @return identifier of the created topic.
    */
   virtual contomap::model::Identifier newTopicRequested(contomap::model::TopicNameValue name, contomap::model::SpacialCoordinate location) = 0;

   /**
    * Called when the default name of a topic shall be changed.
    *
    * @param topicId the identifier of the topic.
    * @param value the new default name of the topic.
    */
   virtual void setTopicNameDefault(contomap::model::Identifier topicId, contomap::model::TopicNameValue value) = 0;

   /**
    * Called when the scoped name of a topic shall be set.
    *
    * @param topicId the identifier of the topic.
    * @param value the name of the topic in the current view scope.
    */
   virtual void setTopicNameInScope(contomap::model::Identifier topicId, contomap::model::TopicNameValue value) = 0;

   /**
    * Called when the scoped name of a topic shall be removed.
    *
    * @param topicId the identifier of the topic.
    */
   virtual void removeTopicNameInScope(contomap::model::Identifier topicId) = 0;

   /**
    * Called when a topic shall receive a further occurrence in the current view scope.
    *
    * @param topicId the identifier of the topic.
    * @param location the point at which the new occurrence shall be put at.
    */
   virtual void newOccurrenceRequested(contomap::model::Identifier topicId, contomap::model::SpacialCoordinate location) = 0;

   /**
    * Called when a new association shall be created.
    *
    * @param location the point at which the association shall be put at.
    * @return identifier of the created association.
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
    */
   virtual void modifySelection(contomap::editor::SelectedType type, contomap::model::Identifier id, contomap::editor::SelectionAction action) = 0;

   /**
    * Called to request a link between the selected items.
    * In case the selection contains one (or more) associations, all the topics from the selected occurrences will be assigned to them.
    * In case the selection contains only occurrences, their respective topics will be linked with one new association.
    */
   virtual void linkSelection() = 0;

   /**
    * Called to request to delete all the selected items.
    */
   virtual void deleteSelection() = 0;

   /**
    * Sets the appearance of all selected items to the provided style.
    *
    * @param appearance the style to use.
    */
   virtual void setAppearanceOfSelection(contomap::model::Style appearance) = 0;

   /**
    * Sets the type of the selected items.
    * @param topicId the identifier of the topic to set.
    */
   virtual void setTypeOfSelection(contomap::model::Identifier topicId) = 0;

   /**
    * Clears the type of the selected items.
    */
   virtual void clearTypeOfSelection() = 0;

   /**
    * Sets the reifier of the selected items.
    * @param topicId the identifier of the topic to set.
    */
   virtual void setReifierOfSelection(contomap::model::Identifier topicId) = 0;

   /**
    * Clears the reifier of the selected items.
    */
   virtual void clearReifierOfSelection() = 0;

   /**
    * Sets the view scope from the current selection.
    */
   virtual void setViewScopeFromSelection() = 0;

   /**
    * Adds to the view scope from the current selection.
    */
   virtual void addToViewScopeFromSelection() = 0;

   /**
    * Sets the view scope to the default.
    */
   virtual void setViewScopeToDefault() = 0;

   /**
    * Sets the view scope to be only the identified topic.
    *
    * @param id identifier of the topic to be the new view scope.
    */
   virtual void setViewScopeTo(contomap::model::Identifier id) = 0;

   /**
    * Adds the given topic identifier to the current view scope.
    *
    * @param id identifier of the topic to be added to the view scope.
    */
   virtual void addToViewScope(contomap::model::Identifier id) = 0;

   /**
    * Removes the given topic identifier from the current view scope.
    * If the remaining view scope becomes empty, the current view scope will be based on the default one.
    * Removing an identifier that is not in the current view scope does nothing.
    *
    * @param id the identifier of the topic to be removed from the view scope.
    */
   virtual void removeFromViewScope(contomap::model::Identifier id) = 0;

   /**
    * With a single occurrence selected, this operation selects the next occurrence of the topic
    * and sets the view scope to that of the new occurrence.
    */
   virtual void cycleSelectedOccurrenceForward() = 0;

   /**
    * With a single occurrence selected, this operation selects the previous occurrence of the topic
    * and sets the view scope to that of the new occurrence.
    */
   virtual void cycleSelectedOccurrenceReverse() = 0;

   /**
    * Selects the closest occurrence of the topic with given identifier.
    * Closeness is determined by the current view scope.
    *
    * @param topicId the identifier of the topic to look for.
    */
   virtual void selectClosestOccurrenceOf(contomap::model::Identifier topicId) = 0;
};

} // namespace contomap::editor
