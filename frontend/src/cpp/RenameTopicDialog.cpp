#include <raygui/raygui.h>

#include "contomap/frontend/RenameTopicDialog.h"

using contomap::editor::InputRequestHandler;
using contomap::frontend::Layout;
using contomap::frontend::RenameTopicDialog;
using contomap::frontend::RenderContext;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::TopicNameValue;

RenameTopicDialog::RenameTopicDialog(InputRequestHandler &inputRequestHandler, Layout const &layout, Identifier topicId, bool isForDefaultName)
   : inputRequestHandler(inputRequestHandler)
   , layout(layout)
   , topicId(topicId)
   , isForDefaultName(isForDefaultName)
{
   newTopicName.fill(0x00);
}

std::unique_ptr<RenameTopicDialog> RenameTopicDialog::forDefaultName(InputRequestHandler &inputRequestHandler, Layout const &layout, Identifier topicId)
{
   return std::unique_ptr<RenameTopicDialog>(new RenameTopicDialog(inputRequestHandler, layout, topicId, true));
}

std::unique_ptr<RenameTopicDialog> RenameTopicDialog::forScopedName(InputRequestHandler &inputRequestHandler, Layout const &layout, Identifier topicId)
{
   return std::unique_ptr<RenameTopicDialog>(new RenameTopicDialog(inputRequestHandler, layout, topicId, false));
}

bool RenameTopicDialog::draw(RenderContext const &context)
{
   auto contentSize = context.getContentSize();
   auto padding = layout.padding();
   Vector2 windowSize { 320, 200 };
   Vector2 windowPos { contentSize.x / 2 - windowSize.x / 2, contentSize.y / 2 - windowSize.y / 2 };

   auto closeDialog = GuiWindowBox(Rectangle { windowPos.x, windowPos.y, windowSize.x, windowSize.y }, "Rename Topic");

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

   auto requested = GuiTextBox(inputBounds, newTopicName.data(), static_cast<int>(newTopicName.size()), true);

   float buttonsHeight = layout.buttonHeight() + padding * 2;
   float buttonsY = windowPos.y + windowSize.y - buttonsHeight;
   Rectangle buttonBounds {
      .x = inputBounds.x,
      .y = buttonsY + padding,
      .width = 150.0f,
      .height = layout.buttonHeight(),
   };

   bool wouldRemoveName = (newTopicName[0] == 0x00);
   if (wouldRemoveName && isForDefaultName)
   {
      GuiDisable();
   }
   if (GuiButton(buttonBounds, (wouldRemoveName && !isForDefaultName) ? "Clear name" : "Rename"))
   {
      requested = true;
   }
   GuiEnable();
   if (requested)
   {
      closeDialog = tryAccept();
   }

   return closeDialog;
}

[[nodiscard]] bool RenameTopicDialog::tryAccept()
{
   if (isForDefaultName)
   {
      auto potentialTopicName = TopicNameValue::from(newTopicName.data());
      if (std::holds_alternative<TopicNameValue>(potentialTopicName))
      {
         inputRequestHandler.setTopicNameDefault(topicId, std::get<TopicNameValue>(potentialTopicName));
         return true;
      }
   }
   else if (newTopicName[0] != 0x00)
   {
      auto potentialTopicName = TopicNameValue::from(newTopicName.data());
      if (std::holds_alternative<TopicNameValue>(potentialTopicName))
      {
         inputRequestHandler.setTopicNameInScope(topicId, std::get<TopicNameValue>(potentialTopicName));
         return true;
      }
   }
   else
   {
      inputRequestHandler.removeTopicNameInScope(topicId);
      return true;
   }
   return false;
}
