#pragma once

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/model/Contomap.h"
#include "contomap/model/Identifiers.h"

namespace contomap::editor
{

/**
 * Editor is the core domain of the application. It contains the logic, regardless of external interfaces.
 */
class Editor : public contomap::editor::InputRequestHandler
{
public:
   Editor();

   void newTopicRequested(contomap::model::TopicNameValue name, contomap::model::SpacialCoordinate location) override;

private:
   contomap::model::Contomap map;
   contomap::model::Identifiers viewScope;
};

} // namespace contomap::editor
