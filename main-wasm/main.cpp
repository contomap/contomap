#include <memory>

#include <emscripten/emscripten.h>
#include <raylib.h>

#include "contomap/application/Application.h"

using contomap::application::Application;
using contomap::frontend::DisplayEnvironment;

/**
 * WasmContext represents the display environment for a browser-embedded WASM application.
 */
class WasmContext : public DisplayEnvironment
{
public:
   /**
    * Constructor.
    */
   WasmContext()
      : app(std::make_unique<Application>(*this))
   {
      instance = app.get();
   }
   ~WasmContext() override
   {
      instance = nullptr;
   }

   /**
    * Initialize and open the window.
    */
   void initWindow()
   {
      app->initWindow();
   }

   /**
    * Request to process a new frame.
    */
   static void nextFrame()
   {
      if (instance != nullptr)
      {
         instance->nextFrame();
      }
   }

   void closeWindow() override
   {
      emscripten_cancel_main_loop();
   }

   DialogResult showSaveAsDialog(std::string const &, std::string &, std::vector<std::string> const &, std::string const &) override
   {
      return DialogResult::NotSupported;
   }

   /**
    * Informs the application to close down.
    */
   void close()
   {
      app->close();
   }

private:
   static Application *instance;
   std::unique_ptr<Application> app;
};
Application *WasmContext::instance = nullptr;

static void nextFrame()
{
   WasmContext::nextFrame();
}

static void runDrawFrameLoop()
{
   int desiredFps = 0; // Recommended to let the browser determine rate.
   int simulateEndlessLoop = EM_TRUE; // Don't return from set_main_loop until emscripten_cancel_main_loop() is called.
   emscripten_set_main_loop(nextFrame, desiredFps, simulateEndlessLoop);
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
