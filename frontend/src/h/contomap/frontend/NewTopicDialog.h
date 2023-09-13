#pragma once

#include <array>

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/frontend/Dialog.h"
#include "contomap/model/TopicNameValue.h"

namespace contomap::frontend
{

/**
 * NewTopicDialog asks for basic properties of a new topic.
 */
class NewTopicDialog : public contomap::frontend::Dialog
{
public:
   explicit NewTopicDialog(contomap::editor::InputRequestHandler &inputRequestHandler);
   ~NewTopicDialog() override = default;

   bool draw(contomap::frontend::RenderContext const &context) override;

private:
   contomap::editor::InputRequestHandler &inputRequestHandler;

   std::array<char, contomap::model::TopicNameValue::maxUtf8Bytes() + 1> newTopicName {};
};

} // namespace contomap::frontend
