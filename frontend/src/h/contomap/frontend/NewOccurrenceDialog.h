#pragma once

#include <array>

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/frontend/Dialog.h"
#include "contomap/frontend/Layout.h"
#include "contomap/model/ContomapView.h"

namespace contomap::frontend
{

/**
 * NewOccurrenceDialog searches for a topic for which to add the occurrence.
 */
class NewOccurrenceDialog : public contomap::frontend::Dialog
{
public:
   /**
    * Constructor.
    *
    * @param inputRequestHandler the handler to call for a new topic.
    * @param view the view on the current map.
    * @param layout the layout to use for drawing.
    * @param location where the topic shall be placed.
    */
   NewOccurrenceDialog(contomap::editor::InputRequestHandler &inputRequestHandler, contomap::model::ContomapView const &view,
      contomap::frontend::Layout const &layout, contomap::model::SpacialCoordinate location);
   ~NewOccurrenceDialog() override = default;

   bool draw(contomap::frontend::RenderContext const &context) override;

private:
   class TopicList
   {
   public:
      TopicList() = default;

      [[nodiscard]] std::optional<contomap::model::Identifier> draw(Rectangle bounds, contomap::infrastructure::Search<contomap::model::Topic const> topics);

   private:
      void guiDrawRectangle(Rectangle rec, int borderWidth, Color borderColor, Color color);
      float guiStyleFloat(int control, int property);

      size_t scrollIndex = 0;
      std::optional<contomap::model::Identifier> selectedTopicId;
      std::optional<contomap::model::Identifier> focusedTopicId;
   };

   contomap::editor::InputRequestHandler &inputRequestHandler;
   contomap::model::ContomapView const &view;
   contomap::frontend::Layout const &layout;

   contomap::model::SpacialCoordinate location;

   std::array<char, contomap::model::TopicNameValue::maxUtf8Bytes() + 1> searchInput {};
   TopicList topicList;
};

} // namespace contomap::frontend
