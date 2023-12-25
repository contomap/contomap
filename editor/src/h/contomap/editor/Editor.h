#pragma once

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/editor/Selection.h"
#include "contomap/editor/View.h"
#include "contomap/editor/ViewScope.h"
#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
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

   void newMap() override;

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
   void setTypeOfSelection(contomap::model::Identifier topicId) override;
   void setTypeOfSelection(contomap::model::TopicNameValue name) override;
   void clearTypeOfSelection() override;
   void setReifierOfSelection(contomap::model::Identifier topicId) override;
   void setReifierOfSelection(contomap::model::TopicNameValue name) override;
   void clearReifierOfSelection() override;
   void moveSelectionBy(contomap::model::SpacialCoordinate::Offset offset) override;
   void setViewScopeFromSelection() override;
   void addToViewScopeFromSelection() override;
   void setViewScopeToDefault() override;
   void setViewScopeTo(contomap::model::Identifier id) override;
   void addToViewScope(contomap::model::Identifier id) override;
   void removeFromViewScope(contomap::model::Identifier id) override;
   void cycleSelectedOccurrenceForward() override;
   void cycleSelectedOccurrenceReverse() override;
   void selectClosestOccurrenceOf(contomap::model::Identifier topicId) override;

   void saveState(contomap::infrastructure::serial::Encoder &encoder, bool withSelection) override;
   [[nodiscard]] bool loadState(contomap::infrastructure::serial::Decoder &decoder) override;

   [[nodiscard]] contomap::editor::ViewScope const &ofViewScope() const override;
   [[nodiscard]] contomap::model::ContomapView const &ofMap() const override;
   [[nodiscard]] contomap::editor::Selection const &ofSelection() const override;

   /**
    * @return the scope used for the default names of topics.
    */
   [[nodiscard]] static contomap::model::Identifiers scopeForTopicDefaultName();

private:
   contomap::model::Identifier newSelfContainedTopic(contomap::model::TopicNameValue const &value);
   void setTopicNameInScope(contomap::model::Identifier topicId, contomap::model::Identifiers const &scope, contomap::model::TopicNameValue value);
   void createAndSelectOccurrence(contomap::model::Topic &topic, contomap::model::SpacialCoordinate location);
   void cycleSelectedOccurrence(bool forward);
   void setViewScopeTo(contomap::model::Identifiers const &ids);
   void verifyViewScopeIsStable();

   static uint8_t const CURRENT_SERIAL_VERSION;

   contomap::model::Contomap map;
   contomap::editor::ViewScope viewScope;
   contomap::editor::Selection selection;
};

} // namespace contomap::editor
