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
class LocateTopicAndActDialog : public contomap::frontend::Dialog
{
public:
   /** Action is the single task to perform upon a selected topic. */
   using Action = std::function<void(contomap::editor::InputRequestHandler &, contomap::model::Identifier)>;

   /**
    * TitledAction is a tuple of a title string and an associated action.
    */
   class TitledAction
   {
   public:
      /**
       * Constructor.
       *
       * @param title the text to display to the user.
       * @param hotkey to use to immediately accept.
       * @param action what to perform upon selection.
       */
      TitledAction(std::string title, int hotkey, Action action)
         : title(std::move(title))
         , hotkey(hotkey)
         , action(std::move(action))
      {
      }

      /**
       * @return the text to display to the user.
       */
      [[nodiscard]] std::string const &getTitle() const
      {
         return title;
      }

      /**
       * @return the hotkey to use to immediately accept.
       */
      [[nodiscard]] int getHotkey() const
      {
         return hotkey;
      }

      /**
       * Perform the associated action.
       *
       * @param handler the handler act upon.
       * @param id the selected topic identifier.
       */
      void act(contomap::editor::InputRequestHandler &handler, contomap::model::Identifier id) const
      {
         action(handler, id);
      }

   private:
      std::string title;
      int hotkey;
      Action action;
   };

   /**
    * Constructor.
    *
    * @param inputRequestHandler the handler to call for a new topic.
    * @param view the view on the current map.
    * @param layout the layout to use for drawing.
    * @param actions the actions to make available to the user.
    */
   LocateTopicAndActDialog(contomap::editor::InputRequestHandler &inputRequestHandler, contomap::model::ContomapView const &view,
      contomap::frontend::Layout const &layout, std::vector<TitledAction> actions);
   ~LocateTopicAndActDialog() override = default;

   bool draw(contomap::frontend::RenderContext const &context) override;

   /**
    * Create an action for adding a new occurrence.
    *
    * @param location at which point the new occurrence shall be placed.
    * @return an action for this dialog.
    */
   static TitledAction newOccurrence(contomap::model::SpacialCoordinate location);
   /**
    * Create an action to set the view scope.
    *
    * @return an action for this dialog.
    */
   static TitledAction setViewScope();
   /**
    * Create an action to add the selected topic to the current view scope.
    *
    * @return an action for this dialog.
    */
   static TitledAction addToViewScope();

private:
   class TopicList
   {
   public:
      explicit TopicList(contomap::model::ContomapView const &view);

      void reset();
      [[nodiscard]] std::optional<contomap::model::Identifier> draw(Rectangle bounds, contomap::infrastructure::Search<contomap::model::Topic const> topics);

   private:
      enum class SelectionOffset
      {
         Previous,
         Next
      };

      void guiDrawRectangle(Rectangle rec, int borderWidth, Color borderColor, Color color);
      float guiStyleFloat(int control, int property);
      void offsetSelection(SelectionOffset offset, size_t visibleCount);

      contomap::model::ContomapView const &view;

      size_t scrollIndex = 0;
      std::optional<size_t> selectedIndex;
      std::optional<contomap::model::Identifier> selectedTopicId;
      std::optional<contomap::model::Identifier> focusedTopicId;
   };

   contomap::editor::InputRequestHandler &inputRequestHandler;
   contomap::model::ContomapView const &view;
   contomap::frontend::Layout const &layout;

   std::vector<TitledAction> actions;

   std::array<char, contomap::model::TopicNameValue::maxUtf8Bytes() + 1> searchInput {};
   TopicList topicList;
};

} // namespace contomap::frontend
