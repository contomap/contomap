#pragma once

namespace contomap::editor
{

class ViewModel
{
public:
   virtual ~ViewModel() = default;

   virtual void showHelpWindow() = 0;
   virtual void hideHelpWindow() = 0;
};

} // namespace contomap::editor
