#pragma once

#include <array>
#include <memory>

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/frontend/Dialog.h"
#include "contomap/frontend/Layout.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/TopicNameValue.h"

namespace contomap::frontend
{

/**
 * NewTopicDialog asks for basic properties of a new topic.
 */
class RenameTopicDialog : public contomap::frontend::Dialog
{
public:
   ~RenameTopicDialog() override = default;

   static std::unique_ptr<RenameTopicDialog> forDefaultName(
      contomap::editor::InputRequestHandler &inputRequestHandler, contomap::frontend::Layout const &layout, contomap::model::Identifier topicId);
   static std::unique_ptr<RenameTopicDialog> forScopedName(
      contomap::editor::InputRequestHandler &inputRequestHandler, contomap::frontend::Layout const &layout, contomap::model::Identifier topicId);

   bool draw(contomap::frontend::RenderContext const &context) override;

private:
   RenameTopicDialog(contomap::editor::InputRequestHandler &inputRequestHandler, contomap::frontend::Layout const &layout, contomap::model::Identifier topicId,
      bool isForDefaultName);

   [[nodiscard]] bool tryAccept();

   contomap::editor::InputRequestHandler &inputRequestHandler;
   contomap::frontend::Layout const &layout;
   contomap::model::Identifier topicId;
   bool isForDefaultName;

   std::array<char, contomap::model::TopicNameValue::maxUtf8Bytes() + 1> newTopicName {};
};

} // namespace contomap::frontend
