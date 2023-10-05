#include <raygui/raygui.h>

#include "contomap/frontend/NewOccurrenceDialog.h"
#include "contomap/model/Topics.h"

using contomap::frontend::NewOccurrenceDialog;
using contomap::frontend::RenderContext;
using contomap::model::Identifier;
using contomap::model::Topic;
using contomap::model::TopicName;
using contomap::model::Topics;

void NewOccurrenceDialog::TopicList::guiDrawRectangle(Rectangle rec, int borderWidth, Color borderColor, Color color)
{
   if (color.a > 0)
   {
      DrawRectangleRec(rec, color);
   }
   if (borderWidth > 0)
   {
      DrawRectangle((int)rec.x, (int)rec.y, (int)rec.width, borderWidth, borderColor);
      DrawRectangle((int)rec.x, (int)rec.y + borderWidth, borderWidth, (int)rec.height - 2 * borderWidth, borderColor);
      DrawRectangle((int)rec.x + (int)rec.width - borderWidth, (int)rec.y + borderWidth, borderWidth, (int)rec.height - 2 * borderWidth, borderColor);
      DrawRectangle((int)rec.x, (int)rec.y + (int)rec.height - borderWidth, (int)rec.width, borderWidth, borderColor);
   }
}

float NewOccurrenceDialog::TopicList::guiStyleFloat(int control, int property)
{
   return static_cast<float>(GuiGetStyle(control, property));
}

void NewOccurrenceDialog::TopicList::reset()
{
   scrollIndex = 0;
   selectedIndex.reset();
   selectedTopicId.reset();
   focusedTopicId.reset();
}

std::optional<Identifier> NewOccurrenceDialog::TopicList::draw(Rectangle bounds, contomap::infrastructure::Search<contomap::model::Topic const> topics)
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
      for (TopicName const &name : topic.allNames())
      {
         totalCount++;
         if (!selectedTopicId.has_value() && selectedIndex.has_value() && (selectedIndex.value() == nameIndex))
         {
            selectedTopicId = topic.getId();
         }
         if (nameIndex < scrollIndex)
         {
            nameIndex++;
            continue;
         }
         nameIndex++;
         if ((itemBounds.y + itemBounds.height) > (bounds.y + bounds.height))
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
         std::string nameText(name.getValue().raw());
         GuiLabel(itemBounds, nameText.c_str());

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

void NewOccurrenceDialog::TopicList::offsetSelection(SelectionOffset offset, size_t visibleCount)
{
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
            scrollIndex = selectedIndex.value() - 2;
         }
         else if (selectedIndex.value() < (scrollIndex + 2) && (scrollIndex > 0))
         {
            scrollIndex--;
         }
      }
      else
      {
         selectedIndex = 0;
         scrollIndex = 0;
      }
      break;
   }
   case SelectionOffset::Next:
   {
      if (selectedIndex.has_value())
      {
         selectedIndex = selectedIndex.value() + 1;
         selectedTopicId.reset();

         bool isAbove = selectedIndex.value() < scrollIndex;
         bool isBelow = selectedIndex.value() >= (scrollIndex + visibleCount);
         if (isAbove || isBelow)
         {
            scrollIndex = selectedIndex.value() - (visibleCount - 3);
         }
         else if (selectedIndex.value() > (scrollIndex + visibleCount - 3))
         {
            scrollIndex++;
         }
      }
      else
      {
         selectedIndex = 0;
         scrollIndex = 0;
      }
      break;
   }
   }
}

NewOccurrenceDialog::NewOccurrenceDialog(contomap::editor::InputRequestHandler &inputRequestHandler, contomap::model::ContomapView const &view,
   contomap::frontend::Layout const &layout, contomap::model::SpacialCoordinate location)
   : inputRequestHandler(inputRequestHandler)
   , view(view)
   , layout(layout)
   , location(location)
{
   searchInput.fill(0x00);
}

bool NewOccurrenceDialog::draw(RenderContext const &context)
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
   bool accept = GuiTextBox(inputBounds, searchInput.data(), static_cast<int>(searchInput.size()), true);
   if (previousData != searchInput)
   {
      topicList.reset();
   }

   Rectangle okButtonBounds {
      .x = inputBounds.x,
      .y = 0.0f,
      .width = 100.0f,
      .height = 30.0f,
   };
   okButtonBounds.y = windowPos.y + windowSize.y - padding - okButtonBounds.height;

   Rectangle listBounds {
      .x = inputBounds.x,
      .y = inputBounds.y + inputBounds.height + padding,
      .width = inputBounds.width,
      .height = okButtonBounds.y - padding,
   };
   listBounds.height -= listBounds.y;

   auto selectedTopicId = topicList.draw(listBounds, view.find(Topics::withANameLike(searchInput.data())));

   if (!selectedTopicId.has_value())
   {
      GuiDisable();
   }
   if (GuiButton(okButtonBounds, "OK"))
   {
      accept = true;
   }
   if (accept && selectedTopicId.has_value())
   {
      inputRequestHandler.newOccurrenceRequested(selectedTopicId.value(), location);
      closeDialog = true;
   }
   GuiEnable();

   return closeDialog;
}
