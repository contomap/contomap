#pragma once

#include <vector>

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/frontend/Dialog.h"
#include "contomap/frontend/Layout.h"
#include "contomap/model/Style.h"

namespace contomap::frontend
{

/**
 * StyleDialog allows modifying a style.
 */
class StyleDialog : public contomap::frontend::Dialog
{
public:
   /**
    * Constructor.
    *
    * @param layout the layout to work with when drawing.
    */
   StyleDialog(contomap::editor::InputRequestHandler &inputRequestHandler, contomap::frontend::Layout const &layout, contomap::model::Style style);
   ~StyleDialog() override = default;

   bool draw(contomap::frontend::RenderContext const &context) override;

private:
   struct ColorSelection
   {
      contomap::model::Style::ColorType type;
      char const *const title;
   };

   static std::vector<ColorSelection> const COLOR_SELECTIONS;

   static contomap::model::Style::Color guiColorPicker(Rectangle bounds, contomap::model::Style::Color color);
   static contomap::model::Style::ColorType colorTypeFromIndex(int value);
   static int indexFromColorType(contomap::model::Style::ColorType value);

   contomap::editor::InputRequestHandler &inputRequestHandler;
   contomap::frontend::Layout layout;
   contomap::model::Style style;

   std::string colorTitles;
   contomap::model::Style::ColorType activeColorType = contomap::model::Style::ColorType::Text;
   bool colorSelectionActive = false;
};

} // namespace contomap::frontend
