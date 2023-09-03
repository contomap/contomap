#include <memory>

#include <emscripten/emscripten.h>
#include <raylib.h>

#include "contomap/frontend/MainWindow.h"

using contomap::frontend::DisplayEnvironment;
using contomap::frontend::MainWindow;

class WasmDisplayEnvironment : public DisplayEnvironment
{
public:
   WasmDisplayEnvironment()
      : mainWindow(std::make_unique<MainWindow>(*this))
   {
      instance = mainWindow.get();
   }
   ~WasmDisplayEnvironment() override
   {
      instance = nullptr;
   }

   static void drawFrame()
   {
      if (instance != nullptr)
      {
         instance->drawFrame();
      }
   }

   void closeWindow() override
   {
      emscripten_cancel_main_loop();
   }

private:
   static MainWindow *instance;
   std::unique_ptr<MainWindow> mainWindow;
};
MainWindow *WasmDisplayEnvironment::instance = nullptr;

void static drawFrame()
{
   WasmDisplayEnvironment::drawFrame();
}

int main()
{
   WasmDisplayEnvironment environment;

   auto initialSize = MainWindow::DEFAULT_SIZE;
   InitWindow(initialSize.getWidth().raw<int>(), initialSize.getHeight().raw<int>(), MainWindow::DEFAULT_TITLE);

   int desiredFps = 0; // Recommended to let the browser determine rate.
   int simulateEndlessLoop = EM_TRUE; // Don't return from set_main_loop until emscripten_cancel_main_loop() is called.
   emscripten_set_main_loop(drawFrame, desiredFps, simulateEndlessLoop);

   CloseWindow();
   return 0;
}
