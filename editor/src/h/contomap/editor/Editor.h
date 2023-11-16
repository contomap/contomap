#pragma once

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/editor/Selection.h"
#include "contomap/editor/View.h"
#include "contomap/model/Contomap.h"
#include "contomap/model/Identifiers.h"

namespace contomap::editor
{

/**
 * Editor is the core domain of the application. It contains the logic, regardless of external interfaces.
 */
class Editor : public contomap::editor::InputRequestHandler, public contomap::editor::View
{
public:
   Editor();

   contomap::model::Identifier newTopicRequested(contomap::model::TopicNameValue name, contomap::model::SpacialCoordinate location) override;
   void setTopicNameDefault(contomap::model::Identifier topicId, contomap::model::TopicNameValue value) override;
   void setTopicNameInScope(contomap::model::Identifier topicId, contomap::model::TopicNameValue value) override;
   void removeTopicNameInScope(contomap::model::Identifier topicId) override;

   void newOccurrenceRequested(contomap::model::Identifier topicId, contomap::model::SpacialCoordinate location) override;
   contomap::model::Identifier newAssociationRequested(contomap::model::SpacialCoordinate location) override;
   void clearSelection() override;
   void modifySelection(contomap::editor::SelectedType type, contomap::model::Identifier id, contomap::editor::SelectionAction action) override;
   void linkSelection() override;
   void deleteSelection() override;
   void setAppearanceOfSelection(contomap::model::Style style) override;
   void setViewScopeFromSelection() override;
   void addToViewScopeFromSelection() override;
   void setViewScopeToDefault() override;
   void setViewScopeTo(contomap::model::Identifier id) override;
   void addToViewScope(contomap::model::Identifier id) override;
   void removeFromViewScope(contomap::model::Identifier id) override;
   void cycleSelectedOccurrenceForward() override;
   void cycleSelectedOccurrenceReverse() override;

   [[nodiscard]] contomap::model::Identifiers const &ofViewScope() const override;
   [[nodiscard]] contomap::model::ContomapView const &ofMap() const override;
   [[nodiscard]] contomap::editor::Selection const &ofSelection() const override;

   /**
    * @return the scope used for the default names of topics.
    */
   [[nodiscard]] static contomap::model::Identifiers scopeForTopicDefaultName();

private:
   void setTopicNameInScope(contomap::model::Identifier topicId, contomap::model::Identifiers const &scope, contomap::model::TopicNameValue value);
   void createAndSelectOccurrence(contomap::model::Topic &topic, contomap::model::SpacialCoordinate location);
   void cycleSelectedOccurrence(bool forward);
   void setViewScopeTo(contomap::model::Identifiers const &ids);
   void verifyViewScopeIsStable();

   contomap::model::Contomap map;
   contomap::model::Identifiers viewScope;
   contomap::editor::Selection selection;
};

} // namespace contomap::editor
