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
   , viewScope(map.getDefaultScopeTopic())
{
}

void Editor::newMap()
{
   map = Contomap::newMap();
   viewScope = contomap::editor::ViewScope(map.getDefaultScopeTopic());
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
   setTopicNameInScope(topicId, viewScope.identifiers(), std::move(value));
}

Identifier Editor::newSelfContainedTopic(TopicNameValue const &value)
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
   topic.value().get().removeNameInScope(viewScope.identifiers());
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
   auto &association = map.newAssociation(viewScope.identifiers(), location);
   selection.setSole<Association>(association);
   return association.getId();
}

void Editor::clearSelection()
{
   selection.clear();
}

void Editor::modifySelection(SelectedType type, Identifier id, SelectionAction action)
{
   auto modify = [this, action]<class T>(std::reference_wrapper<T> entry) {
      if (action == SelectionAction::Set)
      {
         selection.setSole<T>(entry.get());
      }
      else if (action == SelectionAction::Toggle)
      {
         selection.toggle<T>(entry.get());
      }
   };
   if (type == SelectedType::Occurrence)
   {
      auto ids = Identifiers::ofSingle(id);
      for (auto &occurrence : map.findOccurrences(ids))
      {
         modify(occurrence);
      }
   }
   else if (type == SelectedType::Association)
   {
      auto optionalAssociation = map.findAssociation(id);
      if (optionalAssociation.has_value())
      {
         modify(optionalAssociation.value());
      }
   }
   else if (type == SelectedType::Role)
   {
      auto ids = Identifiers::ofSingle(id);
      for (auto &role : map.findRoles(ids))
      {
         modify(role);
      }
   }
}

void Editor::linkSelection()
{
   auto associationIds = selection.of(SelectedType::Association);
   auto occurrenceIds = selection.of(SelectedType::Occurrence);
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
      auto &association = map.newAssociation(viewScope.identifiers(), SpacialCoordinate::absoluteAt(0.0f, 0.0f));
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
      selection.setSole<Association>(association);
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
   for (contomap::model::Styleable &styleable : Selections::allStyleableFrom(selection, map))
   {
      styleable.setAppearance(style);
   }
}

void Editor::setTypeOfSelection(contomap::model::Identifier topicId)
{
   auto optionalTopic = map.findTopic(topicId);
   if (!optionalTopic.has_value())
   {
      return;
   }
   for (contomap::model::Typeable &typeable : Selections::allTypeableFrom(selection, map))
   {
      typeable.setType(optionalTopic.value());
   }
}

void Editor::setTypeOfSelection(contomap::model::TopicNameValue name)
{
   auto topicId = newSelfContainedTopic(name);
   setTypeOfSelection(topicId);
}

void Editor::clearTypeOfSelection()
{
   for (contomap::model::Typeable &typeable : Selections::allTypeableFrom(selection, map))
   {
      typeable.clearType();
   }
}

void Editor::setReifierOfSelection(contomap::model::Identifier topicId)
{
   auto topic = map.findTopic(topicId);
   if (!topic.has_value())
   {
      return;
   }
   for (contomap::model::Reifiable<Topic> &reifiable : Selections::allReifiableFrom(selection, map))
   {
      reifiable.setReifier(topic.value());
   }
}

void Editor::setReifierOfSelection(TopicNameValue name)
{
   auto topicId = newSelfContainedTopic(name);
   setReifierOfSelection(topicId);
}

void Editor::clearReifierOfSelection()
{
   for (contomap::model::Reifiable<Topic> &reifiable : Selections::allReifiableFrom(selection, map))
   {
      reifiable.clearReifier();
   }
}

void Editor::moveSelectionBy(contomap::model::SpacialCoordinate::Offset offset)
{
   for (Occurrence &occurrence : Selections::selectedOccurrences(selection, map))
   {
      occurrence.moveBy(offset);
   }
   for (Association &association : Selections::selectedAssociations(selection, map))
   {
      association.moveBy(offset);
   }
}

void Editor::setViewScopeFromSelection()
{
   auto occurrenceIds = selection.of(SelectedType::Occurrence);
   setViewScopeTo(std::views::common(map.find(Topics::thatOccurAs(occurrenceIds))));
}

void Editor::addToViewScopeFromSelection()
{
   for (Occurrence &occurrence : Selections::selectedOccurrences(selection, map))
   {
      viewScope.add(occurrence.getTopic());
   }
   /*
   auto &occurrenceIds = selection.of(SelectedType::Occurrence);
   for (Topic &topic : map.find(Topics::thatOccurAs(occurrenceIds)))
   {
      viewScope.add(topic);
   }
   */
}

void Editor::setViewScopeToDefault()
{
   setViewScopeTo(std::views::single(std::ref(map.getDefaultScopeTopic())));
}

void Editor::setViewScopeTo(Identifier id)
{
   auto optionalTopic = map.findTopic(id);
   if (!optionalTopic.has_value())
   {
      return;
   }
   setViewScopeTo(std::views::single(std::ref(optionalTopic.value())));
}

void Editor::addToViewScope(Identifier id)
{
   auto optionalTopic = map.findTopic(id);
   if (!optionalTopic.has_value())
   {
      return;
   }
   viewScope.add(optionalTopic.value());
}

void Editor::removeFromViewScope(Identifier id)
{
   auto optionalTopic = map.findTopic(id);
   if (!optionalTopic.has_value())
   {
      return;
   }
   if (!viewScope.remove(optionalTopic.value()))
   {
      return;
   }
   selection.clear();
   verifyViewScopeIsStable();
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
   auto optionalOccurrence = Selections::firstOccurrenceFrom(selection);
   if (!optionalOccurrence.has_value())
   {
      return;
   }
   Occurrence &currentOccurrence = optionalOccurrence.value();
   Topic &topic = currentOccurrence.getTopic();
   auto &nextOccurrence = forward ? topic.nextOccurrenceAfter(currentOccurrence.getId()) : topic.previousOccurrenceBefore(currentOccurrence.getId());
   setViewScopeTo(nextOccurrence.getScope());
   selection.setSole<Occurrence>(nextOccurrence);
}

void Editor::selectClosestOccurrenceOf(Identifier topicId)
{
   auto optionalTopic = map.findTopic(topicId);
   if (!optionalTopic.has_value())
   {
      return;
   }
   Topic &topic = optionalTopic.value();
   auto optionalOccurrence = topic.closestOccurrenceTo(viewScope.identifiers());
   if (!optionalOccurrence.has_value())
   {
      return;
   }
   Occurrence &occurrence = optionalOccurrence.value();
   setViewScopeTo(occurrence.getScope());
   selection.setSole<Occurrence>(occurrence);
}

void Editor::saveState(Encoder &encoder, bool withSelection)
{
   encoder.code("version", CURRENT_SERIAL_VERSION);
   map.encode(encoder);
   viewScope.encode(encoder);
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
   ViewScope newViewScope;
   Selection newSelection;
   uint8_t serialVersion = 0x00;

   try
   {
      decoder.code("version", serialVersion);
      newMap.decode(decoder, serialVersion);
      auto topicResolver = [&newMap](Identifier id) { return newMap.findTopic(id).value(); };
      newViewScope = ViewScope::from(decoder, topicResolver);
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

   map = std::move(newMap);
   viewScope = newViewScope;
   selection = newSelection;
   return true;
}

contomap::editor::ViewScope const &Editor::ofViewScope() const
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
   auto &occurrence = topic.newOccurrence(viewScope.identifiers(), location);
   selection.setSole<Occurrence>(occurrence);
}

void Editor::setViewScopeTo(Identifiers const &ids)
{
   viewScope.clear();
   for (auto const &id : ids)
   {
      viewScope.add(map.findTopic(id).value());
   }
   selection.clear();
}

void Editor::verifyViewScopeIsStable()
{
   if (viewScope.empty())
   {
      viewScope.add(map.getDefaultScopeTopic());
   }
}

Identifiers Editor::scopeForTopicDefaultName()
{
   static Identifiers const empty;
   return empty;
}
