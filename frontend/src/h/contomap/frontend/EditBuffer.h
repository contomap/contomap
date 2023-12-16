#pragma once

#include <raylib.h>

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/frontend/MapCamera.h"

namespace contomap::frontend
{

/**
 * EditBuffer provides a undo/redo buffer for all input request handler operations.
 */
class EditBuffer : public contomap::editor::InputRequestHandler
{
public:
   /**
    * Constructor.
    *
    * @param nested the request handler to forward all requests towards.
    * @param camera the camera for which to handle state.
    */
   EditBuffer(contomap::editor::InputRequestHandler &nested, contomap::frontend::MapCamera &camera);

   /**
    * @return true if at least one operation can be undone.
    */
   [[nodiscard]] bool canUndo() const;
   /**
    * Restores the state before the last operation.
    */
   void undo();
   /**
    * @return true if at least one operation can be redone.
    */
   [[nodiscard]] bool canRedo() const;
   /**
    * Restores the state after the previously undone operation.
    */
   void redo();

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
   void clearTypeOfSelection() override;
   void setReifierOfSelection(contomap::model::Identifier topicId) override;
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

private:
   struct Operation
   {
      Vector2 oldCameraPosition;
      std::vector<uint8_t> stateData;
      Vector2 newCameraPosition;
   };

   class Recorder
   {
   public:
      explicit Recorder(EditBuffer &buffer);
      ~Recorder();

   private:
      EditBuffer &buffer;
      Vector2 oldCameraPosition;
   };

   void reset();
   void recordOperation(Vector2 oldCameraPosition);

   friend Recorder;

   static size_t const UNDO_LIMIT = 10000;

   contomap::editor::InputRequestHandler &nested;
   contomap::frontend::MapCamera &camera;

   std::vector<Operation> operations;
   size_t operationIndex = 0;
};

}
