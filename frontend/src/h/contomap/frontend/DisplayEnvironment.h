#pragma once

#include <vector>

namespace contomap::frontend
{

/**
 * DisplayEnvironment represents the context that the application is running in.
 * Any interaction towards this environment is made available here.
 */
class DisplayEnvironment
{
public:
   /**
    * Status values for requesting a dialog.
    */
   enum class DialogResult
   {
      /** The requested dialog is not supported. */
      NotSupported,
      /** The dialog was cancelled. */
      Cancelled,
      /** The dialog was confirmed. */
      Confirmed
   };

   virtual ~DisplayEnvironment() = default;

   /**
    * Requests to close the window. This causes the frequent calls to draw the window to stop.
    */
   virtual void closeWindow() = 0;

   /**
    * Requests to show/render a dialog that asks for a file name for loading.
    *
    * @param title the title of the dialog.
    * @param filePath the path of the file, a confirmed dialog will have it replaced.
    * @param filter a list of filters, such as {"*.txt", "*.bin"}.
    * @param description a description of the files.
    * @return the result as per user input (and availability).
    */
   // clang-format off
   virtual DialogResult showLoadDialog(
      std::string const &title, std::string &filePath, std::vector<std::string> const &filter, std::string const &description) = 0;
   // clang-format on

   /**
    * Requests to show/render a dialog that asks for a file name for saving.
    *
    * @param title the title of the dialog.
    * @param filePath the path of the file. Input determines proposal, a confirmed dialog will have it replaced.
    * @param filter a list of filters, such as {"*.txt", "*.bin"}.
    * @param description a description of the files.
    * @return the result as per user input (and availability).
    */
   // clang-format off
   virtual DialogResult showSaveAsDialog(
      std::string const &title, std::string &filePath, std::vector<std::string> const &filter, std::string const &description) = 0;
   // clang-format on

   /**
    * Notifies the environment that a file was saved.
    *
    * @param filePath the full path of the saved file.
    */
   virtual void fileSaved(std::string const &filePath) = 0;
};

} // namespace contomap::frontend
