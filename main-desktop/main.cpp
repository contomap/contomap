#include <cmrc/cmrc.hpp>
#include <raylib.h>
#include <tinyfiledialogs/tinyfiledialogs.h>

#include "contomap/application/Application.h"

CMRC_DECLARE(res);

using contomap::application::Application;
using contomap::frontend::DisplayEnvironment;

/**
 * DesktopEnvironment represents the display environment for a standalone application.
 */
class DesktopEnvironment : public DisplayEnvironment
{
public:
   void closeWindow() override
   {
      shouldClose = true;
   }

   DialogResult showLoadDialog(std::string const &title, std::string &filePath, std::vector<std::string> const &filter, std::string const &description) override
   {
      std::vector<char const *> filterPointers;
      for (auto const &f : filter)
      {
         filterPointers.emplace_back(f.c_str());
      }
      int allowMultiSelects = 0;
      auto tempPath = tinyfd_openFileDialog(
         title.c_str(), filePath.c_str(), static_cast<int>(filterPointers.size()), filterPointers.data(), description.c_str(), allowMultiSelects);
      if (tempPath == nullptr)
      {
         return DialogResult::Cancelled;
      }
      filePath = tempPath;
      return DialogResult::Confirmed;
   }

   DialogResult showSaveAsDialog(
      std::string const &title, std::string &filePath, std::vector<std::string> const &filter, std::string const &description) override
   {
      std::vector<char const *> filterPointers;
      for (auto const &f : filter)
      {
         filterPointers.emplace_back(f.c_str());
      }
      auto tempPath
         = tinyfd_saveFileDialog(title.c_str(), filePath.c_str(), static_cast<int>(filterPointers.size()), filterPointers.data(), description.c_str());
      if (tempPath == nullptr)
      {
         return DialogResult::Cancelled;
      }
      filePath = tempPath;
      return DialogResult::Confirmed;
   }

   void fileSaved(std::string const &) override
   {
   }

   /**
    * @return true in case the window should be closed.
    */
   [[nodiscard]] bool shouldCloseWindow() const
   {
      return shouldClose;
   }

private:
   bool shouldClose = false;
};

/**
 * Sets the icon of the current window to the image stored in the resources.
 */
static void setWindowIcon()
{
   auto fs = cmrc::res::get_filesystem();
   auto iconFile = fs.open("desktop/appicon.png");
   std::vector<uint8_t> iconData(iconFile.begin(), iconFile.end());
   auto iconImage = LoadImageFromMemory(".png", iconData.data(), static_cast<int>(iconData.size()));
   SetWindowIcon(iconImage);
   UnloadImage(iconImage);
}

int main()
{
   DesktopEnvironment environment;
   Application app(environment);

   SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);

   app.initWindow();
   setWindowIcon();
   SetTargetFPS(60);
   SetExitKey(KEY_NULL); // Disable default behavior that requests to close window on Escape key.

   while (!environment.shouldCloseWindow())
   {
      if (WindowShouldClose())
      {
         app.closeRequested();
      }
      app.nextFrame();
   }
   app.close();
   return 0;
}
