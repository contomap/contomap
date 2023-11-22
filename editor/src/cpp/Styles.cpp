#include "contomap/editor/Styles.h"

using contomap::editor::Styles;
using contomap::model::ContomapView;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::OptionalIdentifier;
using contomap::model::Style;
using contomap::model::Topic;

Style Styles::resolve(Style const &localStyle, OptionalIdentifier localTypeId, Identifiers const &scope, ContomapView const &view)
{
   return resolve(localStyle, localTypeId, scope, view, 0);
}

Style Styles::resolve(Style const &localStyle, OptionalIdentifier localTypeId, Identifiers const &scope, ContomapView const &view, size_t depth) // NOLINT
{
   if ((depth >= 10) || !localTypeId.isAssigned())
   {
      return localStyle;
   }

   auto potentialTopic = view.findTopic(localTypeId.value());
   if (!potentialTopic.has_value())
   {
      return localStyle;
   }
   Topic const &topic = potentialTopic.value();
   auto scopedView = std::ranges::common_view(topic.occurrencesIn(scope));
   std::vector<std::reference_wrapper<Occurrence const>> occurrences(scopedView.begin(), scopedView.end());
   if (occurrences.empty())
   {
      auto ownView = std::ranges::common_view(topic.occurrencesIn(Identifiers::ofSingle(topic.getId())));
      occurrences.assign(ownView.begin(), ownView.end());
   }
   if (occurrences.empty())
   {
      return localStyle;
   }

   std::sort(occurrences.begin(), occurrences.end(), [](Occurrence const &a, Occurrence const &b) { return a.hasNarrowerScopeThan(b); });
   std::vector<Style> typeStyles;
   Occurrence const &reference = occurrences[0];
   for (Occurrence const &occurrence : occurrences)
   {
      if (occurrence.hasSameScopeSizeAs(reference))
      {
         typeStyles.emplace_back(resolve(occurrence.getAppearance(), occurrence.getType(), scope, view, depth + 1));
      }
   }
   return localStyle.withDefaultsFrom(Style::averageOf(typeStyles));
}
