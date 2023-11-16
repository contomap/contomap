#include <raygui/raygui.h>

#include "contomap/frontend/Colors.h"
#include "contomap/frontend/StyleDialog.h"

using contomap::frontend::Colors;
using contomap::frontend::RenderContext;
using contomap::frontend::StyleDialog;
using contomap::model::Style;

std::vector<StyleDialog::ColorSelection> const StyleDialog::COLOR_SELECTIONS {
   { .type = Style::ColorType::Text, .title = "Text" },
   { .type = Style::ColorType::Line, .title = "Line" },
   { .type = Style::ColorType::Fill, .title = "Fill" },
};

StyleDialog::StyleDialog(contomap::editor::InputRequestHandler &inputRequestHandler, contomap::frontend::Layout const &layout, Style style)
   : inputRequestHandler(inputRequestHandler)
   , layout(layout)
   , style(std::move(style))
{
   for (size_t index = 0; index < COLOR_SELECTIONS.size(); index++)
   {
      if (index > 0)
      {
         colorTitles += ";";
      }
      colorTitles += COLOR_SELECTIONS[index].title;
   }
}

bool StyleDialog::draw(RenderContext const &context)
{
   auto contentSize = context.getContentSize();
   auto padding = layout.padding();
   Vector2 windowSize { 480, 300 };
   Vector2 windowPos { contentSize.x / 2 - windowSize.x / 2, contentSize.y / 2 - windowSize.y / 2 };
   auto windowCloseRequested = GuiWindowBox(Rectangle { windowPos.x, windowPos.y, windowSize.x, windowSize.y }, "Edit Style");

   if (IsKeyPressed(KEY_ESCAPE))
   {
      windowCloseRequested = true;
   }

   Rectangle colorDropdownBounds {
      .x = windowPos.x + padding,
      .y = windowPos.y + layout.windowTitleHeight() + padding,
      .width = windowSize.x - (padding * 2),
      .height = layout.buttonHeight(),
   };

   Rectangle colorCheckboxBounds {
      .x = windowPos.x + padding,
      .y = colorDropdownBounds.y + colorDropdownBounds.height + padding,
      .width = layout.buttonHeight(),
      .height = layout.buttonHeight(),
   };

   bool wantsColorSpecified = GuiCheckBox(colorCheckboxBounds, "Specify", style.has(activeColorType));
   if (wantsColorSpecified != style.has(activeColorType))
   {
      style
         = wantsColorSpecified ? style.with(activeColorType, Style::Color { .red = 0, .green = 0, .blue = 0, .alpha = 0xFF }) : style.without(activeColorType);
   }

   Rectangle colorPickerBounds {
      .x = windowPos.x + padding,
      .y = colorCheckboxBounds.y + colorCheckboxBounds.height + padding,
      .width = windowSize.x - (padding * 2.0f),
      .height = windowPos.y + windowSize.y - (padding * 2.0f) - layout.lineHeight(),
   };
   colorPickerBounds.height -= colorPickerBounds.y;

   if (!style.has(activeColorType))
   {
      GuiDisable();
   }
   auto newColor = guiColorPicker(colorPickerBounds, style.get(activeColorType));
   GuiEnable();
   if (style.has(activeColorType))
   {
      style = style.with(activeColorType, newColor);
   }

   int activeColorIndex = indexFromColorType(activeColorType);
   if (GuiDropdownBox(colorDropdownBounds, colorTitles.c_str(), &activeColorIndex, colorSelectionActive))
   {
      colorSelectionActive = !colorSelectionActive;
   }
   activeColorType = colorTypeFromIndex(activeColorIndex);

   Rectangle bottomSampleBounds {
      .x = windowPos.x + padding,
      .y = windowPos.y + windowSize.y - padding - layout.lineHeight(),
      .width = windowSize.y / 2.0f,
      .height = layout.lineHeight(),
   };
   DrawRectangleRec(bottomSampleBounds, Colors::toUiColor(style.get(Style::ColorType::Fill)));
   DrawRectangleLinesEx(bottomSampleBounds, 2, Colors::toUiColor(style.get(Style::ColorType::Line)));
   DrawText("Sample", static_cast<int>(bottomSampleBounds.x + 3), static_cast<int>(bottomSampleBounds.y + 3), 16,
      Colors::toUiColor(style.get(Style::ColorType::Text)));

   float buttonWidth = 200.0f;
   Rectangle confirmButtonBounds {
      .x = windowPos.x + windowSize.x - buttonWidth - padding,
      .y = windowPos.y + windowSize.y - padding - layout.lineHeight(),
      .width = buttonWidth - padding,
      .height = layout.lineHeight(),
   };
   if (GuiButton(confirmButtonBounds, "Set"))
   {
      inputRequestHandler.setAppearanceOfSelection(style);
      windowCloseRequested = true;
   }

   return windowCloseRequested;
}

Style::Color StyleDialog::guiColorPicker(Rectangle bounds, Style::Color color)
{
   auto outerPadding = static_cast<float>(GuiGetStyle(COLORPICKER, HUEBAR_PADDING)) / 2.0f;
   auto innerPadding = static_cast<float>(GuiGetStyle(COLORPICKER, HUEBAR_PADDING));
   auto alphaHeight = static_cast<float>(GuiGetStyle(COLORPICKER, HUEBAR_WIDTH));
   Rectangle alphaBounds {
      .x = bounds.x + outerPadding,
      .y = bounds.y + bounds.height - alphaHeight - outerPadding,
      .width = bounds.width - (outerPadding * 2.0f),
      .height = alphaHeight,
   };
   Rectangle pickerBounds {
      .x = bounds.x + outerPadding,
      .y = bounds.y + outerPadding,
      .width = bounds.width - static_cast<float>(GuiGetStyle(COLORPICKER, HUEBAR_WIDTH)) - innerPadding - (outerPadding * 2.0f),
      .height = bounds.height - alphaBounds.height - innerPadding - (outerPadding * 2.0f),
   };
   auto result = GuiColorPicker(pickerBounds, nullptr, Colors::toUiColor(color));
   result.a = static_cast<uint8_t>(GuiColorBarAlpha(alphaBounds, nullptr, static_cast<float>(color.alpha) / 255.0f) * 255.0f);
   return Style::Color { .red = result.r, .green = result.g, .blue = result.b, .alpha = result.a };
}

Style::ColorType StyleDialog::colorTypeFromIndex(int value)
{
   return COLOR_SELECTIONS[value].type;
}

int StyleDialog::indexFromColorType(Style::ColorType value)
{
   for (int index = 0; index < static_cast<int>(COLOR_SELECTIONS.size()); index++)
   {
      auto const &selection = COLOR_SELECTIONS[index];
      if (selection.type == value)
      {
         return index;
      }
   }
   return 0;
}
