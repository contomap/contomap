#include "contomap/editor/ViewScope.h"

using contomap::editor::ViewScope;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Topic;

ViewScope::ViewScope(Topic &single)
{
   topics.add(single);
}

ViewScope ViewScope::from(Decoder &coder, std::function<Topic &(Identifier)> const &resolver)
{
   ViewScope result;
   coder.codeArray("viewScope", [&result, &resolver](Decoder &nested, size_t) { result.topics.add(resolver(Identifier::from(nested, ""))); });
   return result;
}

void ViewScope::encode(Encoder &coder) const
{
   auto allTopics = std::ranges::common_view(topics.allReferences());
   coder.codeArray("viewScope", allTopics.begin(), allTopics.end(), [](Encoder &nested, Topic const &topic) { topic.getId().encode(nested, ""); });
}

void ViewScope::clear()
{
   topics.clear();
}

Identifiers ViewScope::identifiers() const
{
   Identifiers result;
   for (Topic const &topic : topics.allReferences())
   {
      result.add(topic.getId());
   }
   return result;
}

bool ViewScope::empty() const
{
   return topics.empty();
}

void ViewScope::add(contomap::model::Topic &topic)
{
   topics.add(topic);
}

bool ViewScope::remove(contomap::model::Topic &topic)
{
   return topics.remove(topic);
}
