#include <memory>

#include <emscripten/emscripten.h>
#include <raylib.h>

#include "contomap/application/Application.h"

using contomap::application::Application;
using contomap::frontend::DisplayEnvironment;

class WasmContext : public DisplayEnvironment
{
public:
   WasmContext()
      : app(std::make_unique<Application>(*this))
   {
      instance = app.get();
   }
   ~WasmContext() override
   {
      instance = nullptr;
   }

   void initWindow()
   {
      app->initWindow();
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

   void close()
   {
      app->close();
   }

private:
   static Application *instance;
   std::unique_ptr<Application> app;
};
Application *WasmContext::instance = nullptr;

void static drawFrame()
{
   WasmContext::drawFrame();
}

static void runDrawFrameLoop()
{
   int desiredFps = 0; // Recommended to let the browser determine rate.
   int simulateEndlessLoop = EM_TRUE; // Don't return from set_main_loop until emscripten_cancel_main_loop() is called.
   emscripten_set_main_loop(drawFrame, desiredFps, simulateEndlessLoop);
}

int main()
{
   WasmContext context;

   SetConfigFlags(FLAG_WINDOW_HIGHDPI);

   context.initWindow();
   runDrawFrameLoop();
   context.close();
   return 0;
}
