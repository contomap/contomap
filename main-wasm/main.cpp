#include <memory>
#include <sstream>

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

   DialogResult showLoadDialog(std::string const &, std::string &, std::vector<std::string> const &, std::string const &) override
   {
      return DialogResult::NotSupported;
   }

   DialogResult showSaveAsDialog(std::string const &, std::string &, std::vector<std::string> const &, std::string const &) override
   {
      return DialogResult::NotSupported;
   }

   void fileSaved(std::string const &filePath) override
   {
      std::ostringstream script;
      auto toIntArray = [&script](std::string const &name, std::string const &text) {
         // Avoid script injections from user by encoding/decoding the input via UTF-8 values.
         script << "let " << name << " = new Int8Array([";
         for (size_t i = 0; i < text.size(); i++)
         {
            if (i > 0)
            {
               script << ",";
            }
            script << std::to_string(static_cast<int>(text[i]));
         }
         script << "]);";
      };
      toIntArray("rawSourcePath", filePath);
      toIntArray("rawTargetPath", GetFileName(filePath.c_str()));
      script << R"EOT(
let utf8Decoder = new TextDecoder();
let sourcePath = utf8Decoder.decode(rawSourcePath);
let sourceData = FS.readFile(sourcePath);
let blob = new Blob([sourceData.buffer], { type: "image/png" });
let element = document.createElement("a");
let url = window.URL.createObjectURL(blob);
element.href = url;
element.download = utf8Decoder.decode(rawTargetPath);
element.click();
window.URL.revokeObjectURL(url);
)EOT";
      emscripten_run_script(script.str().c_str());
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
