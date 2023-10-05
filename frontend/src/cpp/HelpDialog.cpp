#include <string>

#include <raygui/raygui.h>

#include "contomap/editor/VersionInfo.h"
#include "contomap/frontend/HelpDialog.h"

using contomap::frontend::HelpDialog;
using contomap::frontend::RenderContext;

HelpDialog::HelpDialog(contomap::frontend::Layout const &layout)
   : layout(layout)
{
}

bool HelpDialog::draw(RenderContext const &context)
{
   auto contentSize = context.getContentSize();
   auto padding = layout.padding();
   Vector2 windowSize { 320, 200 };
   Vector2 windowPos { contentSize.x / 2 - windowSize.x / 2, contentSize.y / 2 - windowSize.y / 2 };
   auto windowCloseRequested = GuiWindowBox(Rectangle { windowPos.x, windowPos.y, windowSize.x, windowSize.y }, "#193#Help");

   if (IsKeyPressed(KEY_ESCAPE))
   {
      windowCloseRequested = true;
   }

   auto propertiesTitleStart = windowPos.x + padding;
   auto propertiesValueStart = windowPos.x + 50.0f;
   auto propertiesY = windowPos.y + layout.windowTitleHeight() + padding;
   auto propertiesHeight = layout.lineHeight();

   auto addLine = [windowPos, windowSize, propertiesTitleStart, propertiesValueStart, padding, &propertiesY, propertiesHeight](
                     std::string const &title, std::string const &value) {
      GuiLabel(Rectangle { propertiesTitleStart, propertiesY, propertiesValueStart - propertiesTitleStart - padding, propertiesHeight }, title.c_str());
      GuiLabel(Rectangle { propertiesValueStart, propertiesY, windowPos.x + windowSize.x - propertiesValueStart - padding, propertiesHeight }, value.c_str());
      propertiesY += propertiesHeight + padding;
   };

   addLine("Version:", contomap::editor::VersionInfo::GLOBAL.humanReadable());

   auto const codeUrl = "https://github.com/contomap/contomap";
   GuiLabel(Rectangle { propertiesTitleStart, propertiesY, propertiesValueStart - propertiesTitleStart - padding, propertiesHeight }, "Code:");
   if (GuiLabelButton(Rectangle { propertiesValueStart, propertiesY, windowPos.x + windowSize.x - propertiesValueStart - padding, propertiesHeight }, codeUrl))
   {
      OpenURL(codeUrl);
   }

   return windowCloseRequested;
}
