#include <raygui/raygui.h>

#include "contomap/frontend/LocateTopicAndActDialog.h"
#include "contomap/frontend/Names.h"
#include "contomap/model/Topics.h"

using contomap::editor::InputRequestHandler;
using contomap::frontend::LocateTopicAndActDialog;
using contomap::frontend::Names;
using contomap::frontend::RenderContext;
using contomap::model::ContomapView;
using contomap::model::Identifier;
using contomap::model::SpacialCoordinate;
using contomap::model::Topic;
using contomap::model::TopicName;
using contomap::model::Topics;

LocateTopicAndActDialog::TopicList::TopicList(ContomapView const &view)
   : view(view)
{
}

void LocateTopicAndActDialog::TopicList::guiDrawRectangle(Rectangle rec, int borderWidth, Color borderColor, Color color)
{
   if (color.a > 0)
   {
      DrawRectangleRec(rec, color);
   }
   if (borderWidth > 0)
   {
      DrawRectangle(static_cast<int>(rec.x), static_cast<int>(rec.y), static_cast<int>(rec.width), borderWidth, borderColor);
      DrawRectangle(static_cast<int>(rec.x), static_cast<int>(rec.y) + borderWidth, borderWidth, static_cast<int>(rec.height) - 2 * borderWidth, borderColor);
      DrawRectangle(static_cast<int>(rec.x) + static_cast<int>(rec.width) - borderWidth, static_cast<int>(rec.y) + borderWidth, borderWidth,
         static_cast<int>(rec.height) - 2 * borderWidth, borderColor);
      DrawRectangle(
         static_cast<int>(rec.x), static_cast<int>(rec.y) + static_cast<int>(rec.height) - borderWidth, static_cast<int>(rec.width), borderWidth, borderColor);
   }
}

float LocateTopicAndActDialog::TopicList::guiStyleFloat(int control, int property)
{
   return static_cast<float>(GuiGetStyle(control, property));
}

void LocateTopicAndActDialog::TopicList::reset()
{
   scrollIndex = 0;
   selectedIndex.reset();
   selectedTopicId.reset();
   focusedTopicId.reset();
}

std::optional<Identifier> LocateTopicAndActDialog::TopicList::draw(Rectangle bounds, contomap::infrastructure::Search<contomap::model::Topic const> topics)
{
   Rectangle itemBounds = {
      .x = bounds.x + guiStyleFloat(LISTVIEW, LIST_ITEMS_SPACING),
      .y = bounds.y + guiStyleFloat(LISTVIEW, LIST_ITEMS_SPACING) + guiStyleFloat(DEFAULT, BORDER_WIDTH),
      .width = bounds.width - 2.0f * guiStyleFloat(LISTVIEW, LIST_ITEMS_SPACING) - guiStyleFloat(DEFAULT, BORDER_WIDTH),
      .height = guiStyleFloat(LISTVIEW, LIST_ITEMS_HEIGHT),
   };
   float itemIntervalHeight = guiStyleFloat(LISTVIEW, LIST_ITEMS_HEIGHT) + guiStyleFloat(LISTVIEW, LIST_ITEMS_SPACING);

   guiDrawRectangle(
      bounds, GuiGetStyle(DEFAULT, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, 0)), 1.0f), GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

   size_t nameIndex = 0;
   size_t totalCount = 0;
   auto visibleCount = static_cast<size_t>(bounds.height / itemIntervalHeight);
   Vector2 mousePoint = GetMousePosition();

   // This implementation of iterating over the topics and their names immediately draws the entries.
   // This has the following properties:
   // - no intermediate memory needed - regardless of how big the search is.
   // - sorting is not possible, since the full set is not clear. (likewise: no scroll bar possible)
   // - it depends on a stable iteration order over time. Should maps in model be reordered, this would jump around.

   if (IsKeyReleased(KEY_UP))
   {
      offsetSelection(SelectionOffset::Previous, visibleCount);
   }
   else if (IsKeyReleased(KEY_DOWN))
   {
      offsetSelection(SelectionOffset::Next, visibleCount);
   }

   auto currentlySelectedTopicId = selectedTopicId;

   bool somethingFocused = false;
   for (Topic const &topic : topics)
   {
      for (std::string const &name : Names::forDisplay(topic, view.getDefaultScope()))
      {
         totalCount++;
         if (!selectedTopicId.has_value() && selectedIndex.has_value() && (selectedIndex.value() == nameIndex))
         {
            selectedTopicId = topic.getId();
         }
         if (bool isNotYetVisible = nameIndex < scrollIndex; isNotYetVisible)
         {
            nameIndex++;
            continue;
         }
         nameIndex++;
         if (bool isBeyondVisibleArea = (itemBounds.y + itemBounds.height) > (bounds.y + bounds.height); isBeyondVisibleArea)
         {
            continue;
         }

         if (CheckCollisionPointRec(mousePoint, itemBounds))
         {
            focusedTopicId = topic.getId();
            somethingFocused = true;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
               selectedIndex = nameIndex;
               selectedTopicId = topic.getId();
            }
         }
         if (currentlySelectedTopicId == topic.getId())
         {
            guiDrawRectangle(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_PRESSED)), 1.0f),
               Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_PRESSED)), 1.0f));
         }
         else if (focusedTopicId == topic.getId())
         {
            guiDrawRectangle(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_FOCUSED)), 1.0f),
               Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_FOCUSED)), 1.0f));
         }
         GuiLabel(itemBounds, name.c_str());

         itemBounds.y += itemIntervalHeight;
      }
   }
   if (!somethingFocused)
   {
      focusedTopicId.reset();
   }
   if (selectedIndex.has_value() && (selectedIndex.value() >= totalCount))
   {
      if (totalCount > 0)
      {
         selectedIndex = totalCount - 1;
      }
      else
      {
         selectedIndex.reset();
      }
   }
   {
      int wheelMove = static_cast<int>(GetMouseWheelMove());
      if (wheelMove > 0)
      {
         scrollIndex -= std::min(scrollIndex, static_cast<size_t>(wheelMove));
      }
      else
      {
         scrollIndex += -wheelMove;
      }

      if (scrollIndex >= totalCount)
      {
         scrollIndex = (totalCount > 0) ? totalCount - 1 : 0;
      }
   }

   return currentlySelectedTopicId;
}

void LocateTopicAndActDialog::TopicList::offsetSelection(SelectionOffset offset, size_t visibleCount)
{
   auto offsetScrollIndex = [this](size_t add, size_t deduct) {
      scrollIndex = selectedIndex.value() + add;
      scrollIndex -= std::min(scrollIndex, deduct);
   };
   auto selectAndScrollToFirst = [this]() {
      selectedIndex = 0;
      scrollIndex = 0;
   };
   selectedTopicId.reset();
   switch (offset)
   {
   case SelectionOffset::Previous:
   {
      if (selectedIndex.has_value() && (selectedIndex.value() > 0))
      {
         selectedIndex = selectedIndex.value() - 1;

         bool isAbove = selectedIndex.value() < scrollIndex;
         bool isBelow = selectedIndex.value() >= (scrollIndex + visibleCount);
         if (isAbove || isBelow)
         {
            offsetScrollIndex(0, 2);
         }
         else if (selectedIndex.value() < (scrollIndex + 2) && (scrollIndex > 0))
         {
            scrollIndex--;
         }
      }
      else
      {
         selectAndScrollToFirst();
      }
      break;
   }
   case SelectionOffset::Next:
   {
      if (selectedIndex.has_value())
      {
         selectedIndex = selectedIndex.value() + 1;

         bool isAbove = selectedIndex.value() < scrollIndex;
         bool isBelow = selectedIndex.value() >= (scrollIndex + visibleCount);
         if (isAbove || isBelow)
         {
            offsetScrollIndex(3, visibleCount);
         }
         else if (selectedIndex.value() > (scrollIndex + visibleCount - 3))
         {
            scrollIndex++;
         }
      }
      else
      {
         selectAndScrollToFirst();
      }
      break;
   }
   }
}

LocateTopicAndActDialog::LocateTopicAndActDialog(contomap::editor::InputRequestHandler &inputRequestHandler, ContomapView const &view,
   contomap::frontend::Layout const &layout, std::vector<TitledAction> actions)
   : inputRequestHandler(inputRequestHandler)
   , view(view)
   , layout(layout)
   , actions(std::move(actions))
   , topicList(view)
{
   searchInput.fill(0x00);
}

bool LocateTopicAndActDialog::draw(RenderContext const &context)
{
   auto contentSize = context.getContentSize();
   auto padding = layout.padding();
   Vector2 windowSize { 640, 480 };
   Vector2 windowPos { contentSize.x / 2 - windowSize.x / 2, contentSize.y / 2 - windowSize.y / 2 };

   auto closeDialog = GuiWindowBox(Rectangle { windowPos.x, windowPos.y, windowSize.x, windowSize.y }, "Find Topic");

   if (IsKeyPressed(KEY_ESCAPE))
   {
      closeDialog = true;
   }

   auto height = layout.lineHeight();
   Rectangle inputBounds {
      .x = windowPos.x + padding,
      .y = windowPos.y + layout.windowTitleHeight() + padding,
      .width = windowSize.x - padding * 2,
      .height = height,
   };
   auto previousData = searchInput;
   GuiTextBox(inputBounds, searchInput.data(), static_cast<int>(searchInput.size()), true);
   if (previousData != searchInput)
   {
      topicList.reset();
   }

   float buttonsHeight = layout.buttonHeight() + padding * 2;
   float buttonsY = windowPos.y + windowSize.y - buttonsHeight;

   Rectangle listBounds {
      .x = inputBounds.x,
      .y = inputBounds.y + inputBounds.height + padding,
      .width = inputBounds.width,
      .height = buttonsY,
   };
   listBounds.height -= listBounds.y;

   auto selectedTopicId = topicList.draw(listBounds, view.find(Topics::withANameLike(searchInput.data())));

   if (!selectedTopicId.has_value())
   {
      GuiDisable();
   }

   Rectangle buttonBounds {
      .x = inputBounds.x,
      .y = buttonsY + padding,
      .width = 0.0f,
      .height = layout.buttonHeight(),
   };

   Font font = GuiGetFont();
   auto fontSize = static_cast<float>(GuiGetStyle(DEFAULT, TEXT_SIZE));
   auto spacing = static_cast<float>(GuiGetStyle(DEFAULT, TEXT_SPACING));
   auto textPadding = static_cast<float>(GuiGetStyle(BUTTON, TEXT_PADDING));
   auto buttonBorderWidth = static_cast<float>(GuiGetStyle(BUTTON, BORDER_WIDTH));
   float extraPadding = layout.buttonHeight(); // necessary as zero apparently results in characters cut away.

   for (auto const &action : actions)
   {
      bool acceptByKey = (action.getHotkey() != 0) && IsKeyReleased(action.getHotkey()) && IsKeyDown(KEY_LEFT_ALT);
      if (!acceptByKey && (actions.size() == 1))
      {
         acceptByKey = IsKeyReleased(KEY_ENTER);
      }
      auto textSize = MeasureTextEx(font, action.getTitle().c_str(), fontSize, spacing);
      buttonBounds.width = textSize.x + (textPadding * 2.0f) + (buttonBorderWidth * 2.0f) + extraPadding;
      if ((GuiButton(buttonBounds, action.getTitle().c_str()) || acceptByKey) && selectedTopicId.has_value())
      {
         action.act(inputRequestHandler, selectedTopicId.value());
         closeDialog = true;
         break;
      }
      buttonBounds.x += buttonBounds.width + padding;
   }
   GuiEnable();

   return closeDialog;
}

LocateTopicAndActDialog::TitledAction LocateTopicAndActDialog::newOccurrence(SpacialCoordinate location)
{
   return { "Add occurrence", KEY_O, [location](InputRequestHandler &handler, Identifier id) { handler.newOccurrenceRequested(id, location); } };
}

LocateTopicAndActDialog::TitledAction LocateTopicAndActDialog::setViewScope()
{
   return { std::string("#") + std::to_string(ICON_BOX_DOTS_BIG) + "#Set view scope", KEY_S,
      [](InputRequestHandler &handler, Identifier id) { handler.setViewScopeTo(id); } };
}

LocateTopicAndActDialog::TitledAction LocateTopicAndActDialog::addToViewScope()
{
   return { "Add to view scope", KEY_V, [](InputRequestHandler &handler, Identifier id) { handler.addToViewScope(id); } };
}
