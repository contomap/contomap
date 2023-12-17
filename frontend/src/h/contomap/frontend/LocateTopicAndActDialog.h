#pragma once

#include <array>
#include <utility>

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
   /** TopicAction is the single task to perform upon a selected topic. */
   using TopicAction = std::function<void(contomap::editor::InputRequestHandler &, contomap::model::Identifier)>;

   /** NameAction is the single task to perform upon an entered name. */
   using NameAction = std::function<void(contomap::editor::InputRequestHandler &, contomap::model::TopicNameValue const &)>;

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
      TitledAction(std::string title, int hotkey, TopicAction action)
         : title(std::move(title))
         , hotkey(hotkey)
         , requiredInput(RequiredInput::Topic)
         , action(fullActionFor(action))
      {
      }

      /**
       * Constructor.
       *
       * @param title the text to display to the user.
       * @param hotkey to use to immediately accept.
       * @param action what to perform upon selection.
       */
      TitledAction(std::string title, int hotkey, NameAction action)
         : title(std::move(title))
         , hotkey(hotkey)
         , requiredInput(RequiredInput::Name)
         , action(fullActionFor(action))
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
       * @return true if the action is only allowed if a topic is resolved.
       */
      [[nodiscard]] bool requiresTopic() const
      {
         return requiredInput == RequiredInput::Topic;
      }

      /**
       * @return true if the action is only allowed if a name is available.
       */
      [[nodiscard]] bool requiresName() const
      {
         return requiredInput == RequiredInput::Name;
      }

      /**
       * Perform the associated action.
       *
       * @param handler the handler act upon.
       * @param id the selected topic identifier.
       * @return whether the action was performed with the given parameters.
       */
      [[nodiscard]] bool act(contomap::editor::InputRequestHandler &handler, contomap::model::Identifier id) const
      {
         return action(handler, Input { .selectedTopicId { id }, .enteredName {} });
      }

      /**
       * Perform the associated action.
       *
       * @param handler the handler act upon.
       * @param id the selected topic identifier.
       * @param name the entered name.
       * @return whether the action was performed with the given parameters.
       */
      [[nodiscard]] bool act(contomap::editor::InputRequestHandler &handler, std::optional<contomap::model::Identifier> id,
         std::optional<contomap::model::TopicNameValue> name) const
      {
         return action(handler, Input { .selectedTopicId = id, .enteredName = std::move(name) });
      }

   private:
      enum class RequiredInput
      {
         Topic,
         Name
      };

      struct Input
      {
         /** The identifier of the selected topic, if available. */
         std::optional<contomap::model::Identifier> selectedTopicId;
         /** The name of the text input, if available. */
         std::optional<contomap::model::TopicNameValue> enteredName;
      };

      using FullAction = std::function<bool(contomap::editor::InputRequestHandler &, Input)>;

      [[nodiscard]] static FullAction fullActionFor(TopicAction const &action)
      {
         return [action](contomap::editor::InputRequestHandler &handler, Input input) {
            if (!input.selectedTopicId.has_value())
            {
               return false;
            }
            action(handler, input.selectedTopicId.value());
            return true;
         };
      }

      [[nodiscard]] static FullAction fullActionFor(NameAction const &action)
      {
         return [action](contomap::editor::InputRequestHandler &handler, Input input) {
            if (!input.enteredName.has_value())
            {
               return false;
            }
            action(handler, input.enteredName.value());
            return true;
         };
      }

      std::string title;
      int hotkey;
      RequiredInput requiredInput;
      FullAction action;
   };

   /**
    * Constructor.
    *
    * @param inputRequestHandler the handler to call for a user action.
    * @param view the view on the current map.
    * @param layout the layout to use for drawing.
    * @param actions the actions to make available to the user.
    */
   LocateTopicAndActDialog(contomap::editor::InputRequestHandler &inputRequestHandler, contomap::model::ContomapView const &view,
      contomap::frontend::Layout const &layout, std::vector<TitledAction> actions);
   ~LocateTopicAndActDialog() override = default;

   bool draw(contomap::frontend::RenderContext const &context) override;

   /**
    * Create an action for creating a new topic.
    *
    * @param location at which point the first occurrence of the new topic shall be placed.
    * @return an action for this dialog.
    */
   [[nodiscard]] static TitledAction newTopic(contomap::model::SpacialCoordinate location);
   /**
    * Create an action for adding a new occurrence.
    *
    * @param location at which point the new occurrence shall be placed.
    * @return an action for this dialog.
    */
   [[nodiscard]] static TitledAction newOccurrence(contomap::model::SpacialCoordinate location);
   /**
    * Create an action to set the view scope.
    *
    * @return an action for this dialog.
    */
   [[nodiscard]] static TitledAction setViewScope();
   /**
    * Create an action to add the selected topic to the current view scope.
    *
    * @return an action for this dialog.
    */
   [[nodiscard]] static TitledAction addToViewScope();

   /**
    * Sets the type of the selected items.
    *
    * @return an action for this dialog.
    */
   [[nodiscard]] static TitledAction setTypeOfSelection();

   /**
    * Sets the reifier of the selected items.
    *
    * @return an action for this dialog.
    */
   [[nodiscard]] static TitledAction setReifierOfSelection();

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
