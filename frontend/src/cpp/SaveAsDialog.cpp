#include <cstring>
#include <string>

#include <raygui/raygui.h>

#include "contomap/frontend/SaveAsDialog.h"

using contomap::frontend::DisplayEnvironment;
using contomap::frontend::RenderContext;
using contomap::frontend::SaveAsDialog;

SaveAsDialog::SaveAsDialog(contomap::frontend::DisplayEnvironment &environment, contomap::frontend::Layout const &layout, std::string const &proposedFilePath)
   : environment(environment)
   , layout(layout)
   , proposedFilePath(proposedFilePath)
{
   std::strncpy(newFilePath.data(), proposedFilePath.c_str(), newFilePath.size());
}

bool SaveAsDialog::draw(RenderContext const &context)
{
   std::string title("Save As...");

   bool closeDialog = false;

   if (auto result = environment.showSaveAsDialog(title, proposedFilePath, { "*.png" }, "Contomap image files");
       result != DisplayEnvironment::DialogResult::NotSupported)
   {
      closeDialog = true;
   }
   else
   {
      auto contentSize = context.getContentSize();
      auto padding = layout.padding();
      Vector2 windowSize { 320, 200 };
      Vector2 windowPos { contentSize.x / 2 - windowSize.x / 2, contentSize.y / 2 - windowSize.y / 2 };

      closeDialog = GuiWindowBox(Rectangle { windowPos.x, windowPos.y, windowSize.x, windowSize.y }, title.c_str());
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

      auto requested = GuiTextBox(inputBounds, newFilePath.data(), static_cast<int>(newFilePath.size()), true);

      float buttonsHeight = layout.buttonHeight() + padding * 2;
      float buttonsY = windowPos.y + windowSize.y - buttonsHeight;
      Rectangle buttonBounds {
         .x = inputBounds.x,
         .y = buttonsY + padding,
         .width = 150.0f,
         .height = layout.buttonHeight(),
      };

      bool isNameEmpty = (newFilePath[0] == 0x00);
      if (isNameEmpty)
      {
         GuiDisable();
      }
      if (GuiButton(buttonBounds, "Save"))
      {
         requested = true;
      }
      GuiEnable();
      if (requested)
      {
         closeDialog = true; // TODO tryAccept();
      }
   }
   return closeDialog;
}
