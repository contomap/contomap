#pragma once

#include "contomap/editor/InputRequestHandler.h"

namespace contomap::editor
{

/**
 * Editor is the core domain of the application. It contains the logic, regardless of external interfaces.
 */
class Editor : public contomap::editor::InputRequestHandler
{
public:
   void newTopicRequested(contomap::model::TopicNameValue name) override;

private:
};

} // namespace contomap::editor
