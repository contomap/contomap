#include <string>

#include <raygui/raygui.h>

#include "contomap/frontend/LoadDialog.h"

using contomap::frontend::DisplayEnvironment;
using contomap::frontend::LoadDialog;
using contomap::frontend::RenderContext;

LoadDialog::LoadDialog(contomap::frontend::DisplayEnvironment &environment, LoadFunction loadFunction)
   : environment(environment)
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
      Vector2 windowSize { 320, 200 };
      Vector2 windowPos { contentSize.x / 2 - windowSize.x / 2, contentSize.y / 2 - windowSize.y / 2 };

      closeDialog = GuiMessageBox(Rectangle { windowPos.x, windowPos.y, windowSize.x, windowSize.y }, title.c_str(),
                       "Please drag'n'drop a Contomap .png file into the window.", "Cancel")
         >= 0;
      if (IsKeyPressed(KEY_ESCAPE))
      {
         closeDialog = true;
      }

      if (IsFileDropped())
      {
         FilePathList droppedFiles = LoadDroppedFiles();
         loadFunction(droppedFiles.paths[0]);
         UnloadDroppedFiles(droppedFiles);
         closeDialog = true;
      }
   }
   return closeDialog;
}
