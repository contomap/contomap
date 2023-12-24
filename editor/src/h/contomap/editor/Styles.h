#pragma once

#include <optional>

#include "contomap/model/Style.h"
#include "contomap/model/Topic.h"

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
    * @param localType the type set for the local item.
    * @param scope the scope within which to resolve the style.
    * @return the final style.
    */
   [[nodiscard]] static contomap::model::Style resolve(contomap::model::Style const &localStyle,
      std::optional<std::reference_wrapper<contomap::model::Topic const>> localType, contomap::model::Identifiers const &scope);

private:
   [[nodiscard]] static contomap::model::Style resolve(contomap::model::Style const &localStyle,
      std::optional<std::reference_wrapper<contomap::model::Topic const>> localType, contomap::model::Identifiers const &scope, size_t depth);
};

} // namespace contomap::editor
