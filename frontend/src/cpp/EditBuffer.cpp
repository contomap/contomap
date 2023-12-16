#include "contomap/frontend/EditBuffer.h"
#include "contomap/infrastructure/serial/BinaryDecoder.h"
#include "contomap/infrastructure/serial/BinaryEncoder.h"

using contomap::frontend::EditBuffer;
using contomap::frontend::MapCamera;
using contomap::infrastructure::serial::BinaryDecoder;
using contomap::infrastructure::serial::BinaryEncoder;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Identifier;
using contomap::model::SpacialCoordinate;
using contomap::model::Style;
using contomap::model::TopicNameValue;

EditBuffer::Recorder::Recorder(EditBuffer &buffer)
   : buffer(buffer)
   , oldCameraPosition(buffer.camera.getCurrentPosition())
{
}

EditBuffer::Recorder::~Recorder()
{
   buffer.recordOperation(oldCameraPosition);
}

EditBuffer::EditBuffer(InputRequestHandler &nested, MapCamera &camera)
   : nested(nested)
   , camera(camera)
{
   Recorder rec(*this);
}

bool EditBuffer::canUndo() const
{
   return operationIndex > 1;
}

void EditBuffer::undo()
{
   if (!canUndo())
   {
      return;
   }
   auto const &lastOperation = operations[operationIndex - 1];
   auto const &secondLastOperation = operations[operationIndex - 2];
   BinaryDecoder decoder(secondLastOperation.stateData.data(), secondLastOperation.stateData.data() + secondLastOperation.stateData.size());
   if (nested.loadState(decoder))
   {
      operationIndex--;
      camera.panTo(lastOperation.oldCameraPosition);
   }
}

bool EditBuffer::canRedo() const
{
   return operationIndex < operations.size();
}

void EditBuffer::redo()
{
   if (!canRedo())
   {
      return;
   }
   auto const &nextOperation = operations[operationIndex];
   BinaryDecoder decoder(nextOperation.stateData.data(), nextOperation.stateData.data() + nextOperation.stateData.size());
   if (nested.loadState(decoder))
   {
      operationIndex++;
      camera.panTo(nextOperation.newCameraPosition);
   }
}

void EditBuffer::reset()
{
   operations.clear();
   operationIndex = 0;
   camera.panTo(MapCamera::HOME_POSITION);
   Recorder rec(*this);
}

void EditBuffer::recordOperation(Vector2 oldCameraPosition)
{
   Operation operation;
   BinaryEncoder encoder;
   nested.saveState(encoder, true);
   operation.oldCameraPosition = oldCameraPosition;
   operation.stateData = encoder.getData();
   operation.newCameraPosition = camera.getCurrentPosition();
   if ((operationIndex > 0) && (operations[operationIndex - 1].stateData == operation.stateData))
   {
      return;
   }
   if (operationIndex == operations.size())
   {
      operations.emplace_back(std::move(operation));
      if (operations.size() > UNDO_LIMIT)
      {
         operations.erase(operations.begin());
      }
      else
      {
         operationIndex++;
      }
   }
   else
   {
      operations[operationIndex] = std::move(operation);
      operationIndex++;
      operations.resize(operationIndex);
   }
}

void EditBuffer::newMap()
{
   nested.newMap();
   reset();
}

Identifier EditBuffer::newTopicRequested(TopicNameValue name, SpacialCoordinate location)
{
   Recorder rec(*this);
   return nested.newTopicRequested(name, location);
}

void EditBuffer::setTopicNameDefault(Identifier topicId, TopicNameValue value)
{
   Recorder rec(*this);
   nested.setTopicNameDefault(topicId, value);
}

void EditBuffer::setTopicNameInScope(Identifier topicId, TopicNameValue value)
{
   Recorder rec(*this);
   nested.setTopicNameInScope(topicId, value);
}

void EditBuffer::removeTopicNameInScope(Identifier topicId)
{
   Recorder rec(*this);
   nested.removeTopicNameInScope(topicId);
}

void EditBuffer::newOccurrenceRequested(Identifier topicId, SpacialCoordinate location)
{
   Recorder rec(*this);
   nested.newOccurrenceRequested(topicId, location);
}

Identifier EditBuffer::newAssociationRequested(SpacialCoordinate location)
{
   Recorder rec(*this);
   return nested.newAssociationRequested(location);
}

void EditBuffer::clearSelection()
{
   nested.clearSelection();
}

void EditBuffer::modifySelection(contomap::editor::SelectedType type, Identifier id, contomap::editor::SelectionAction action)
{
   nested.modifySelection(type, id, action);
}

void EditBuffer::linkSelection()
{
   Recorder rec(*this);
   nested.linkSelection();
}

void EditBuffer::deleteSelection()
{
   Recorder rec(*this);
   nested.deleteSelection();
}

void EditBuffer::setAppearanceOfSelection(Style style)
{
   Recorder rec(*this);
   nested.setAppearanceOfSelection(style);
}

void EditBuffer::setTypeOfSelection(Identifier topicId)
{
   Recorder rec(*this);
   nested.setTypeOfSelection(topicId);
}

void EditBuffer::clearTypeOfSelection()
{
   Recorder rec(*this);
   nested.clearTypeOfSelection();
}

void EditBuffer::setReifierOfSelection(Identifier topicId)
{
   Recorder rec(*this);
   nested.setReifierOfSelection(topicId);
}

void EditBuffer::clearReifierOfSelection()
{
   Recorder rec(*this);
   nested.clearReifierOfSelection();
}

void EditBuffer::moveSelectionBy(SpacialCoordinate::Offset offset)
{
   Recorder rec(*this);
   nested.moveSelectionBy(offset);
}

void EditBuffer::setViewScopeFromSelection()
{
   Recorder rec(*this);
   nested.setViewScopeFromSelection();
   camera.panTo(MapCamera::HOME_POSITION);
}

void EditBuffer::addToViewScopeFromSelection()
{
   Recorder rec(*this);
   nested.addToViewScopeFromSelection();
}

void EditBuffer::setViewScopeToDefault()
{
   Recorder rec(*this);
   nested.setViewScopeToDefault();
   camera.panTo(MapCamera::HOME_POSITION);
}

void EditBuffer::setViewScopeTo(Identifier id)
{
   Recorder rec(*this);
   nested.setViewScopeTo(id);
}

void EditBuffer::addToViewScope(Identifier id)
{
   Recorder rec(*this);
   nested.addToViewScope(id);
}

void EditBuffer::removeFromViewScope(Identifier id)
{
   Recorder rec(*this);
   nested.removeFromViewScope(id);
}

void EditBuffer::cycleSelectedOccurrenceForward()
{
   nested.cycleSelectedOccurrenceForward();
}

void EditBuffer::cycleSelectedOccurrenceReverse()
{
   nested.cycleSelectedOccurrenceReverse();
}

void EditBuffer::selectClosestOccurrenceOf(Identifier topicId)
{
   nested.selectClosestOccurrenceOf(topicId);
}

void EditBuffer::saveState(contomap::infrastructure::serial::Encoder &encoder, bool withSelection)
{
   nested.saveState(encoder, withSelection);
}

bool EditBuffer::loadState(contomap::infrastructure::serial::Decoder &decoder)
{
   if (!nested.loadState(decoder))
   {
      return false;
   }
   reset();
   return true;
}
