#pragma once

#include "contomap/model/ContomapView.h"
#include "contomap/model/Style.h"

namespace contomap::editor
{

/**
 * Styles provides helper functions for resolving style.
 */
class Styles
{
public:
   /**
    * Resolves the style for a local item.
    * @param localStyle the style set for the local item.
    * @param localTypeId the type set for the local item.
    * @param scope the scope within which to resolve the style.
    * @param view the view from which to retrieve further types.
    * @return the final style.
    */
   [[nodiscard]] static contomap::model::Style resolve(contomap::model::Style const &localStyle, contomap::model::OptionalIdentifier localTypeId,
      contomap::model::Identifiers const &scope, contomap::model::ContomapView const &view);

   // TODO: upper variant should be dropped

   [[nodiscard]] static contomap::model::Style resolve(contomap::model::Style const &localStyle,
      std::optional<std::reference_wrapper<contomap::model::Topic const>> localType, contomap::model::Identifiers const &scope);

private:
   [[nodiscard]] static contomap::model::Style resolve(contomap::model::Style const &localStyle,
      std::optional<std::reference_wrapper<contomap::model::Topic const>> localType, contomap::model::Identifiers const &scope, size_t depth);
};

} // namespace contomap::editor
