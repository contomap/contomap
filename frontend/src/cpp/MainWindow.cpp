#include <string>

#include <raygui/raygui.h>
#include <raylib.h>

#include "contomap/editor/VersionInfo.h"
#include "contomap/frontend/MainWindow.h"

using contomap::frontend::MainWindow;

MainWindow::LengthInPixel::LengthInPixel(MainWindow::LengthInPixel::ValueType value)
   : value(value)
{
}

MainWindow::LengthInPixel MainWindow::LengthInPixel::of(LengthInPixel::ValueType value)
{
   return LengthInPixel(value);
}

MainWindow::Size MainWindow::Size::ofPixel(LengthInPixel::ValueType width, LengthInPixel::ValueType height)
{
   return { LengthInPixel::of(width), LengthInPixel::of(height) };
}

MainWindow::Size::Size(MainWindow::LengthInPixel width, MainWindow::LengthInPixel height)
   : width(width)
   , height(height)
{
}

MainWindow::LengthInPixel MainWindow::Size::getWidth() const
{
   return width;
}

MainWindow::LengthInPixel MainWindow::Size::getHeight() const
{
   return height;
}

MainWindow::Size const MainWindow::DEFAULT_SIZE = MainWindow::Size::ofPixel(800, 450);
char const MainWindow::DEFAULT_TITLE[] = "contomap";

MainWindow::MainWindow(DisplayEnvironment &environment, contomap::editor::InputRequestHandler &inputRequestHandler)
   : environment(environment)
   , inputRequestHandler(inputRequestHandler)
{
}

contomap::editor::ViewModel &MainWindow::viewModel()
{
   return viewModelState;
}

void MainWindow::init()
{
   auto initialSize = MainWindow::DEFAULT_SIZE;
   InitWindow(initialSize.getWidth().raw<int>(), initialSize.getHeight().raw<int>(), MainWindow::DEFAULT_TITLE);
}

void MainWindow::close()
{
   CloseWindow();
}

void MainWindow::closeRequested()
{
   environment.closeWindow();
}

void MainWindow::drawFrame()
{
   BeginDrawing();

   drawBackground();
   drawMap();
   drawUserInterface();

   EndDrawing();
}

void MainWindow::drawBackground()
{
   ClearBackground(WHITE);
}

void MainWindow::drawMap()
{
}

void MainWindow::drawUserInterface()
{
   if (viewModelState.anyWindowShown())
   {
      GuiLock();
      GuiDisableTooltip();
   }
   else
   {
      GuiUnlock();
      GuiEnableTooltip();
   }

   auto dpiScale = GetWindowScaleDPI();
   Vector2 contentSize { static_cast<float>(GetRenderWidth()) / dpiScale.x, static_cast<float>(GetRenderHeight()) / dpiScale.y };
   auto buttonHeight = 24.0f;
   auto padding = 2.0f;
   GuiPanel(Rectangle { 0, 0, contentSize.x, buttonHeight + (padding * 2.0f) }, nullptr);
   GuiSetTooltip("Show help window");
   if (GuiButton(Rectangle { contentSize.x - (padding + buttonHeight) * 1, padding, buttonHeight, buttonHeight }, GuiIconText(ICON_HELP, nullptr)))
   {
      inputRequestHandler.helpWindowShowRequested();
   }

   if (viewModelState.anyWindowShown())
   {
      DrawRectangle(0, 0, static_cast<int>(contentSize.x), static_cast<int>(contentSize.y), Fade(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), 0.8f));
      GuiUnlock();
   }

   if (viewModelState.helpWindowShown)
   {
      Vector2 windowSize { 320, 200 };
      Vector2 windowPos { contentSize.x / 2 - windowSize.x / 2, contentSize.y / 2 - windowSize.y / 2 };
      auto windowCloseRequested = GuiWindowBox(Rectangle { windowPos.x, windowPos.y, windowSize.x, windowSize.y }, "#193#Help");

      if (IsKeyPressed(KEY_ESCAPE))
      {
         windowCloseRequested = true;
      }

      auto propertiesTitleStart = windowPos.x + padding;
      auto propertiesValueStart = windowPos.x + 50.0f;
      auto propertiesY = windowPos.y + 40.0f;
      auto propertiesHeight = 20.0f;

      auto addLine = [windowPos, windowSize, propertiesTitleStart, propertiesValueStart, padding, &propertiesY, propertiesHeight](
                        std::string const &title, std::string const &value) {
         GuiLabel(Rectangle { propertiesTitleStart, propertiesY, propertiesValueStart - propertiesTitleStart - padding, propertiesHeight }, title.c_str());
         GuiLabel(
            Rectangle { propertiesValueStart, propertiesY, windowPos.x + windowSize.x - propertiesValueStart - padding, propertiesHeight }, value.c_str());
         propertiesY += propertiesHeight + padding;
      };

      addLine("Version:", contomap::editor::VersionInfo::GLOBAL.humanReadable());

      auto const codeUrl = "https://github.com/contomap/contomap";
      GuiLabel(Rectangle { propertiesTitleStart, propertiesY, propertiesValueStart - propertiesTitleStart - padding, propertiesHeight }, "Code:");
      if (GuiLabelButton(
             Rectangle { propertiesValueStart, propertiesY, windowPos.x + windowSize.x - propertiesValueStart - padding, propertiesHeight }, codeUrl))
      {
         OpenURL(codeUrl);
      }

      if (windowCloseRequested)
      {
         inputRequestHandler.helpWindowHideRequested();
      }
   }
}
