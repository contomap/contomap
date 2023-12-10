#include <cstring>
#include <string>

#include <raygui/raygui.h>

#include "contomap/frontend/LoadDialog.h"

using contomap::frontend::DisplayEnvironment;
using contomap::frontend::LoadDialog;
using contomap::frontend::RenderContext;

LoadDialog::LoadDialog(contomap::frontend::DisplayEnvironment &environment, contomap::frontend::Layout const &layout, LoadFunction loadFunction)
   : environment(environment)
   , layout(layout)
   , loadFunction(std::move(loadFunction))
{
   newFilePath.fill(0x00);
}

bool LoadDialog::draw(RenderContext const &context)
{
   std::string title("Load...");

   bool closeDialog = false;

   std::string filePath;
   if (auto result = environment.showLoadDialog(title, filePath, { "*.png" }, "Contomap image files"); result != DisplayEnvironment::DialogResult::NotSupported)
   {
      loadFunction(filePath);
      closeDialog = true;
   }
   else
   {
      auto contentSize = context.getContentSize();
      // auto padding = layout.padding();
      Vector2 windowSize { 320, 200 };
      Vector2 windowPos { contentSize.x / 2 - windowSize.x / 2, contentSize.y / 2 - windowSize.y / 2 };

      closeDialog = GuiWindowBox(Rectangle { windowPos.x, windowPos.y, windowSize.x, windowSize.y }, title.c_str());
      if (IsKeyPressed(KEY_ESCAPE))
      {
         closeDialog = true;
      }

      // TODO: Probably wait for file dropped.

      /*
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
      if (GuiButton(buttonBounds, "Load"))
      {
         requested = true;
      }
      GuiEnable();
      if (requested)
      {
         loadFunction(newFilePath.data());
         closeDialog = true;
      }
      */
   }
   return closeDialog;
}
