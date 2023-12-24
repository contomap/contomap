#include "contomap/editor/Editor.h"
#include "contomap/editor/Selections.h"
#include "contomap/model/Topics.h"

using contomap::editor::Editor;
using contomap::editor::SelectedType;
using contomap::editor::SelectionAction;
using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Association;
using contomap::model::Contomap;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::Role;
using contomap::model::SpacialCoordinate;
using contomap::model::Style;
using contomap::model::Topic;
using contomap::model::TopicNameValue;
using contomap::model::Topics;

uint8_t const Editor::CURRENT_SERIAL_VERSION = 0x00;

Editor::Editor()
   : map(Contomap::newMap())
{
   viewScope.add(map.getDefaultScope());
}

void Editor::newMap()
{
   map = Contomap::newMap();
   viewScope = Identifiers::ofSingle(map.getDefaultScope());
   selection.clear();
}

Identifier Editor::newTopicRequested(TopicNameValue name, SpacialCoordinate location)
{
   auto &topic = map.newTopic();
   static_cast<void>(topic.newName(scopeForTopicDefaultName(), name));
   createAndSelectOccurrence(topic, location);
   return topic.getId();
}

void Editor::setTopicNameDefault(Identifier topicId, TopicNameValue value)
{
   setTopicNameInScope(topicId, scopeForTopicDefaultName(), std::move(value));
}

void Editor::setTopicNameInScope(Identifier topicId, TopicNameValue value)
{
   setTopicNameInScope(topicId, viewScope, std::move(value));
}

Identifier Editor::newSelfContainedTopic(TopicNameValue value)
{
   auto &topic = map.newTopic();
   static_cast<void>(topic.newName(scopeForTopicDefaultName(), value));
   static_cast<void>(topic.newOccurrence(Identifiers::ofSingle(topic.getId()), SpacialCoordinate::absoluteAt(0.0f, 0.0f)));
   return topic.getId();
}

void Editor::setTopicNameInScope(Identifier topicId, Identifiers const &scope, TopicNameValue value)
{
   auto topic = map.findTopic(topicId);
   if (!topic.has_value())
   {
      return;
   }
   topic.value().get().setNameInScope(scope, std::move(value));
}

void Editor::removeTopicNameInScope(Identifier topicId)
{
   auto topic = map.findTopic(topicId);
   if (!topic.has_value())
   {
      return;
   }
   topic.value().get().removeNameInScope(viewScope);
}

void Editor::newOccurrenceRequested(Identifier topicId, SpacialCoordinate location)
{
   auto topic = map.findTopic(topicId);
   if (!topic.has_value())
   {
      return;
   }
   createAndSelectOccurrence(topic.value(), location);
}

Identifier Editor::newAssociationRequested(SpacialCoordinate location)
{
   auto &association = map.newAssociation(viewScope, location);
   selection.setSole(SelectedType::Association, association.getId());
   return association.getId();
}

void Editor::clearSelection()
{
   selection.clear();
}

void Editor::modifySelection(SelectedType type, Identifier id, SelectionAction action)
{
   if (action == SelectionAction::Set)
   {
      selection.setSole(type, id);
   }
   else if (action == SelectionAction::Toggle)
   {
      selection.toggle(type, id);
   }
}

void Editor::linkSelection()
{
   auto &associationIds = selection.of(SelectedType::Association);
   auto &occurrenceIds = selection.of(SelectedType::Occurrence);
   if (!associationIds.empty())
   {
      for (auto associationId : associationIds)
      {
         Association &association = map.findAssociation(associationId).value();
         auto topics = map.find(Topics::thatOccurAs(occurrenceIds));
         for (Topic &topic : topics)
         {
            static_cast<void>(topic.newRole(association));
         }
      }
   }
   else if (!occurrenceIds.empty())
   {
      auto &association = map.newAssociation(viewScope, SpacialCoordinate::absoluteAt(0.0f, 0.0f));
      auto topics = map.find(Topics::thatOccurAs(occurrenceIds));
      float x = 0.0f;
      float y = 0.0f;
      size_t count = 0;
      for (Topic &topic : topics)
      {
         static_cast<void>(topic.newRole(association));

         for (Occurrence const &occurrence : topic.findOccurrences(occurrenceIds))
         {
            auto occurrenceLocation = occurrence.getLocation().getSpacial().getAbsoluteReference();
            x += occurrenceLocation.X();
            y += occurrenceLocation.Y();
            count++;
         }
      }
      association.moveTo(SpacialCoordinate::absoluteAt(x / static_cast<float>(count), y / static_cast<float>(count)));
      selection.setSole(SelectedType::Association, association.getId());
   }
}

void Editor::deleteSelection()
{
   map.deleteRoles(selection.of(SelectedType::Role));
   map.deleteAssociations(selection.of(SelectedType::Association));
   map.deleteOccurrences(selection.of(SelectedType::Occurrence));
   selection.clear();
   verifyViewScopeIsStable();
}

void Editor::setAppearanceOfSelection(Style style)
{
   if (auto const &ids = selection.of(SelectedType::Occurrence); !ids.empty())
   {
      for (auto &occurrence : map.findOccurrences(ids))
      {
         occurrence.get().setAppearance(style);
      }
   }
   if (auto const &ids = selection.of(SelectedType::Association); !ids.empty())
   {
      for (auto id : ids)
      {
         Association &association = map.findAssociation(id).value();
         association.setAppearance(style);
      }
   }
   if (auto const &ids = selection.of(SelectedType::Role); !ids.empty())
   {
      for (Role &role : map.findRoles(ids))
      {
         role.setAppearance(style);
      }
   }
}

void Editor::setTypeOfSelection(contomap::model::Identifier topicId)
{
   auto optionalTopic = map.findTopic(topicId);
   if (!optionalTopic.has_value())
   {
      return;
   }
   if (auto const &ids = selection.of(SelectedType::Occurrence); !ids.empty())
   {
      for (auto &occurrence : map.findOccurrences(ids))
      {
         occurrence.get().setType(optionalTopic.value());
      }
   }
   if (auto const &ids = selection.of(SelectedType::Association); !ids.empty())
   {
      for (auto id : ids)
      {
         Association &association = map.findAssociation(id).value();
         association.setType(optionalTopic.value());
      }
   }
   if (auto const &ids = selection.of(SelectedType::Role); !ids.empty())
   {
      for (Role &role : map.findRoles(ids))
      {
         role.setType(optionalTopic.value());
      }
   }
}

void Editor::setTypeOfSelection(contomap::model::TopicNameValue name)
{
   auto topicId = newSelfContainedTopic(name);
   setTypeOfSelection(topicId);
}

void Editor::clearTypeOfSelection()
{
   if (auto const &ids = selection.of(SelectedType::Occurrence); !ids.empty())
   {
      for (auto &occurrence : map.findOccurrences(ids))
      {
         occurrence.get().clearType();
      }
   }
   if (auto const &ids = selection.of(SelectedType::Association); !ids.empty())
   {
      for (auto id : ids)
      {
         Association &association = map.findAssociation(id).value();
         association.clearType();
      }
   }
   if (auto const &ids = selection.of(SelectedType::Role); !ids.empty())
   {
      for (Role &role : map.findRoles(ids))
      {
         role.clearType();
      }
   }
}

void Editor::setReifierOfSelection(contomap::model::Identifier topicId)
{
   auto topic = map.findTopic(topicId);
   if (!topic.has_value())
   {
      return;
   }
   if (auto const &ids = selection.of(SelectedType::Occurrence); !ids.empty())
   {
      for (auto &occurrence : map.findOccurrences(ids))
      {
         occurrence.get().setReifier(topic.value());
      }
   }
   if (auto const &ids = selection.of(SelectedType::Association); !ids.empty())
   {
      for (auto id : ids)
      {
         Association &association = map.findAssociation(id).value();
         association.setReifier(topic.value());
      }
   }
   if (auto const &ids = selection.of(SelectedType::Role); !ids.empty())
   {
      for (Role &role : map.findRoles(ids))
      {
         role.setReifier(topic.value());
      }
   }
}

void Editor::setReifierOfSelection(TopicNameValue name)
{
   auto topicId = newSelfContainedTopic(name);
   setReifierOfSelection(topicId);
}

void Editor::clearReifierOfSelection()
{
   if (auto const &ids = selection.of(SelectedType::Occurrence); !ids.empty())
   {
      for (auto &occurrence : map.findOccurrences(ids))
      {
         occurrence.get().clearReifier();
      }
   }
   if (auto const &ids = selection.of(SelectedType::Association); !ids.empty())
   {
      for (auto id : ids)
      {
         Association &association = map.findAssociation(id).value();
         association.clearReifier();
      }
   }
   if (auto const &ids = selection.of(SelectedType::Role); !ids.empty())
   {
      for (Role &role : map.findRoles(ids))
      {
         role.clearReifier();
      }
   }
}

void Editor::moveSelectionBy(contomap::model::SpacialCoordinate::Offset offset)
{
   if (auto const &ids = selection.of(SelectedType::Occurrence); !ids.empty())
   {
      for (auto &occurrence : map.findOccurrences(ids))
      {
         occurrence.get().moveBy(offset);
      }
   }
   if (auto const &ids = selection.of(SelectedType::Association); !ids.empty())
   {
      for (auto id : ids)
      {
         Association &association = map.findAssociation(id).value();
         association.moveBy(offset);
      }
   }
}

void Editor::setViewScopeFromSelection()
{
   auto &occurrenceIds = selection.of(SelectedType::Occurrence);
   Identifiers newViewScope;
   for (Topic &topic : map.find(Topics::thatOccurAs(occurrenceIds)))
   {
      newViewScope.add(topic.getId());
   }
   if (!newViewScope.empty())
   {
      setViewScopeTo(newViewScope);
   }
}

void Editor::addToViewScopeFromSelection()
{
   auto &occurrenceIds = selection.of(SelectedType::Occurrence);
   Identifiers newViewScope;
   for (Topic &topic : map.find(Topics::thatOccurAs(occurrenceIds)))
   {
      addToViewScope(topic.getId());
   }
}

void Editor::setViewScopeToDefault()
{
   setViewScopeTo(Identifiers::ofSingle(map.getDefaultScope()));
}

void Editor::setViewScopeTo(Identifier id)
{
   if (!map.findTopic(id).has_value())
   {
      return;
   }
   setViewScopeTo(Identifiers::ofSingle(id));
}

void Editor::addToViewScope(Identifier id)
{
   if (!map.findTopic(id).has_value())
   {
      return;
   }
   viewScope.add(id);
}

void Editor::removeFromViewScope(Identifier id)
{
   if (!viewScope.remove(id))
   {
      return;
   }
   if (viewScope.empty())
   {
      viewScope.add(map.getDefaultScope());
   }
   selection.clear();
}

void Editor::cycleSelectedOccurrenceForward()
{
   cycleSelectedOccurrence(true);
}

void Editor::cycleSelectedOccurrenceReverse()
{
   cycleSelectedOccurrence(false);
}

void Editor::cycleSelectedOccurrence(bool forward)
{
   if (!selection.hasSoleEntryFor(SelectedType::Occurrence))
   {
      return;
   }
   Identifier originalOccurrenceId = *selection.of(SelectedType::Occurrence).begin();
   for (Topic &topic : map.find(Topics::thatOccurAs(Identifiers::ofSingle(originalOccurrenceId))))
   {
      auto const &nextOccurrence = forward ? topic.nextOccurrenceAfter(originalOccurrenceId) : topic.previousOccurrenceBefore(originalOccurrenceId);
      viewScope = nextOccurrence.getScope();
      selection.setSole(SelectedType::Occurrence, nextOccurrence.getId());
   }
}

void Editor::selectClosestOccurrenceOf(Identifier topicId)
{
   auto optionalTopic = map.findTopic(topicId);
   if (!optionalTopic.has_value())
   {
      return;
   }
   Topic &topic = optionalTopic.value();
   auto optionalOccurrence = topic.closestOccurrenceTo(viewScope);
   if (!optionalOccurrence.has_value())
   {
      return;
   }
   Occurrence const &occurrence = optionalOccurrence.value();
   viewScope = occurrence.getScope();
   selection.setSole(SelectedType::Occurrence, occurrence.getId());
}

void Editor::saveState(Encoder &encoder, bool withSelection)
{
   encoder.code("version", CURRENT_SERIAL_VERSION);
   map.encode(encoder);
   viewScope.encode(encoder, "viewScope");
   {
      Coder::Scope selectionScope(encoder, "selection");
      uint8_t selectionFlag = withSelection ? 0x01 : 0x00;
      encoder.code("present", selectionFlag);
      if (selectionFlag != 0x00)
      {
         selection.encode(encoder);
      }
   }
}

bool Editor::loadState(Decoder &decoder)
{
   Contomap newMap = contomap::model::Contomap::newMap();
   Identifiers newViewScope;
   Selection newSelection;
   uint8_t serialVersion = 0x00;

   try
   {
      decoder.code("version", serialVersion);
      newMap.decode(decoder, serialVersion);
      newViewScope.decode(decoder, "viewScope");
      {
         Coder::Scope selectionScope(decoder, "selection");
         uint8_t selectionFlag = 0x00;
         decoder.code("present", selectionFlag);
         if (selectionFlag != 0x00)
         {
            auto occurrenceResolver = [&newMap](Identifier id) { return *newMap.findOccurrences(Identifiers::ofSingle(id)).begin(); };
            auto associationResolver = [&newMap](Identifier id) { return newMap.findAssociation(id).value(); };
            auto roleResolver = [&newMap](Identifier id) { return *newMap.findRoles(Identifiers::ofSingle(id)).begin(); };
            newSelection = Selection::from(decoder, serialVersion, occurrenceResolver, associationResolver, roleResolver);
         }
      }
   }
   catch (std::exception &)
   {
      return false;
   }

   // TODO: verify new state is consistent -> proper references

   map = std::move(newMap);
   viewScope = newViewScope;
   selection = newSelection;
   return true;
}

Identifiers const &Editor::ofViewScope() const
{
   return viewScope;
}

contomap::model::ContomapView const &Editor::ofMap() const
{
   return map;
}

contomap::editor::Selection const &Editor::ofSelection() const
{
   return selection;
}

void Editor::createAndSelectOccurrence(contomap::model::Topic &topic, contomap::model::SpacialCoordinate location)
{
   auto &occurrence = topic.newOccurrence(viewScope, location);
   selection.setSole(SelectedType::Occurrence, occurrence.getId());
}

void Editor::setViewScopeTo(contomap::model::Identifiers const &ids)
{
   viewScope = ids;
   selection.clear();
}

void Editor::verifyViewScopeIsStable()
{
   Identifiers unknownIds;

   for (auto id : viewScope)
   {
      if (!map.findTopic(id).has_value())
      {
         unknownIds.add(id);
      }
   }
   for (auto id : unknownIds)
   {
      removeFromViewScope(id);
   }
}

Identifiers Editor::scopeForTopicDefaultName()
{
   static Identifiers const empty;
   return empty;
}
