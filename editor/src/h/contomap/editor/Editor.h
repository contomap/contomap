#pragma once

#include "contomap/editor/InputRequestHandler.h"
#include "contomap/editor/Selection.h"
#include "contomap/editor/View.h"
#include "contomap/model/Contomap.h"
#include "contomap/model/Identifiers.h"

namespace contomap::editor
{

/**
 * Editor is the core domain of the application. It contains the logic, regardless of external interfaces.
 */
class Editor : public contomap::editor::InputRequestHandler, public contomap::editor::View
{
public:
   Editor();

   void newTopicRequested(contomap::model::TopicNameValue name, contomap::model::SpacialCoordinate location) override;
   void newAssociationRequested(contomap::model::SpacialCoordinate location) override;
   void modifySelectionOfOccurrence(contomap::model::Identifier id, contomap::editor::SelectionAction action, contomap::editor::SelectionMode mode) override;
   void modifySelectionOfAssociation(contomap::model::Identifier id, contomap::editor::SelectionAction action, contomap::editor::SelectionMode mode) override;

   [[nodiscard]] contomap::model::Identifiers const &ofViewScope() const override;
   [[nodiscard]] contomap::model::ContomapView const &ofMap() const override;
   [[nodiscard]] contomap::editor::Selection const &ofSelection() const override;

private:
   contomap::model::Contomap map;
   contomap::model::Identifiers viewScope;
   contomap::editor::Selection selection;
};

} // namespace contomap::editor
