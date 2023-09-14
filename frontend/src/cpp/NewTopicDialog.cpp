#include <raygui/raygui.h>

#include "contomap/frontend/NewTopicDialog.h"

using contomap::frontend::NewTopicDialog;
using contomap::frontend::RenderContext;
using contomap::model::TopicNameValue;

NewTopicDialog::NewTopicDialog(contomap::editor::InputRequestHandler &inputRequestHandler, contomap::frontend::Layout const &layout)
   : inputRequestHandler(inputRequestHandler)
   , layout(layout)
{
}

bool NewTopicDialog::draw(RenderContext const &context)
{
   auto contentSize = context.getContentSize();
   auto padding = layout.padding();
   Vector2 windowSize { 320, 200 };
   Vector2 windowPos { contentSize.x / 2 - windowSize.x / 2, contentSize.y / 2 - windowSize.y / 2 };

   auto closeDialog = GuiWindowBox(Rectangle { windowPos.x, windowPos.y, windowSize.x, windowSize.y }, "New Topic");

   if (IsKeyPressed(KEY_ESCAPE))
   {
      closeDialog = true;
   }

   auto height = layout.lineHeight();

   auto accepted = GuiTextBox(
      Rectangle { .x = windowPos.x + padding, .y = windowPos.y + layout.windowTitleHeight() + padding, .width = windowSize.x - padding * 2, .height = height },
      newTopicName.data(), static_cast<int>(newTopicName.size()), true);

   auto potentialTopicName = TopicNameValue::from(newTopicName.data());
   if (accepted && std::holds_alternative<TopicNameValue>(potentialTopicName))
   {
      inputRequestHandler.newTopicRequested(std::get<TopicNameValue>(potentialTopicName));
      closeDialog = true;
   }

   return closeDialog;
}
