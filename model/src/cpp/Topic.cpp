#include "contomap/model/Topic.h"

using contomap::infrastructure::Link;
using contomap::infrastructure::Links;
using contomap::infrastructure::Search;
using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
using contomap::model::Association;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::Reified;
using contomap::model::Role;
using contomap::model::SpacialCoordinate;
using contomap::model::Topic;
using contomap::model::TopicName;

Topic::Topic(Identifier id)
   : id(id)
{
}

Topic::~Topic()
{
   clearReified();
}

Topic &Topic::refine()
{
   return *this;
}

void Topic::encodeRelated(Encoder &coder) const
{
   Coder::Scope scope(coder, "related");
   coder.codeArray("names", names.begin(), names.end(), [](Encoder &nested, auto const &kvp) {
      Coder::Scope nameScope(nested, "");
      kvp.first.encode(nested, "id");
      kvp.second.encode(nested);
   });
   coder.codeArray("occurrences", occurrences.begin(), occurrences.end(), [](Encoder &nested, auto const &kvp) {
      Coder::Scope nestedScope(nested, "");
      kvp.first.encode(nested, "id");
      kvp.second->encode(nested);
   });
   coder.codeArray("roles", roles.begin(), roles.end(), [](Encoder &nested, auto const &kvp) {
      Coder::Scope nestedScope(nested, "");
      kvp.first.encode(nested, "id");
      kvp.second->role().encode(nested);
   });
}

void Topic::decodeRelated(
   Decoder &coder, uint8_t version, std::function<Topic &(Identifier)> topicResolver, std::function<Association &(Identifier)> associationResolver)
{
   Coder::Scope scope(coder, "related");
   coder.codeArray("names", [this, version](Decoder &nested, size_t) {
      Coder::Scope nameScope(nested, "");
      auto nameId = Identifier::from(nested, "id");
      auto name = TopicName::from(nested, version, nameId);
      names.emplace(nameId, name);
   });
   coder.codeArray("occurrences", [this, version, &topicResolver](Decoder &nested, size_t) {
      Coder::Scope nestedScope(nested, "");
      Identifier occurrenceId = Identifier::from(nested, "id");
      occurrences.emplace(occurrenceId, Occurrence::from(nested, version, occurrenceId, *this, topicResolver));
   });
   coder.codeArray("roles", [this, version, &topicResolver, &associationResolver](Decoder &nested, size_t) {
      Coder::Scope nestedScope(nested, "");
      Identifier roleId = Identifier::from(nested, "id");
      auto role = Role::from(nested, version, roleId, topicResolver, associationResolver);
      auto it = roles.find(roleId);
      it->second->own(std::move(role));
   });
}

Identifier Topic::getId() const
{
   return id;
}

TopicName &Topic::newName(Identifiers scope, contomap::model::TopicNameValue const &value)
{
   auto nameId = Identifier::random();
   auto it = names.emplace(nameId, TopicName(nameId, std::move(scope), value));
   return it.first->second;
}

Search<TopicName const> Topic::allNames() const // NOLINT
{
   for (auto const &[_, name] : names)
   {
      co_yield name;
   }
}

void Topic::setNameInScope(Identifiers const &scope, TopicNameValue value)
{
   auto existingName = findNameByScope(scope);
   if (existingName.has_value())
   {
      existingName.value().get().setValue(std::move(value));
   }
   else
   {
      static_cast<void>(newName(scope, value));
   }
}

void Topic::removeNameInScope(Identifiers const &scope)
{
   auto existingName = findNameByScope(scope);
   if (!existingName.has_value())
   {
      return;
   }
   names.erase(existingName.value().get().getId());
}

Occurrence &Topic::newOccurrence(Identifiers scope, SpacialCoordinate location)
{
   auto occurrenceId = Identifier::random();
   auto it = occurrences.emplace(occurrenceId, std::make_unique<Occurrence>(occurrenceId, *this, std::move(scope), location));
   return *it.first->second;
}

bool Topic::removeOccurrence(Identifier occurrenceId)
{
   return occurrences.erase(occurrenceId) > 0;
}

Role &Topic::newRole(Association &association)
{
   auto roleId = Identifier::random();
   auto role = std::make_unique<Role>(roleId, *this, association);
   auto it = roles.find(roleId);
   it->second->own(std::move(role));
   return it->second->role();
}

std::unique_ptr<Link<Topic>> Topic::link(Role &role, std::function<void()> topicUnlinked)
{
   Identifier roleId = role.getId();
   auto links = Links::between(*this, std::move(topicUnlinked), role, [this, roleId]() { roles.erase(roleId); });
   roles.emplace(roleId, std::make_unique<RoleEntry>(std::move(links.second)));
   return std::move(links.first);
}

void Topic::removeRolesOf(Association &association)
{
   Identifiers toRemove;
   for (auto const &[roleId, entry] : roles)
   {
      if (entry->role().getParent() == association.getId())
      {
         toRemove.add(roleId);
      }
   }
   std::erase_if(roles, [&toRemove](auto const &kvp) {
      auto const &[_, entry] = kvp;
      return toRemove.contains(entry->role().getId());
   });
}

void Topic::removeRole(Identifier roleId)
{
   roles.erase(roleId);
}

bool Topic::isIn(Identifiers const &scope) const
{
   return std::any_of(occurrences.begin(), occurrences.end(), [&scope](auto const &kvp) { return kvp.second->isIn(scope); });
}

bool Topic::occursAsAnyOf(Identifiers const &occurrenceIds) const
{
   return std::any_of(occurrences.begin(), occurrences.end(), [&occurrenceIds](auto const &kvp) { return occurrenceIds.contains(kvp.first); });
}

bool Topic::isWithoutOccurrences() const
{
   return occurrences.empty();
}

Search<Occurrence const> Topic::occurrencesIn(contomap::model::Identifiers scope) const // NOLINT
{
   for (auto const &[_, occurrence] : occurrences)
   {
      if (occurrence->isIn(scope))
      {
         co_yield *occurrence;
      }
   }
}

std::optional<std::reference_wrapper<Occurrence const>> Topic::closestOccurrenceTo(contomap::model::Identifiers const &scope) const
{
   auto scopedView = std::ranges::common_view(occurrencesIn(scope));
   std::vector<std::reference_wrapper<Occurrence const>> candidates(scopedView.begin(), scopedView.end());
   if (candidates.empty())
   {
      for (auto const &occurrence : occurrences)
      {
         candidates.emplace_back(*occurrence.second);
      }
   }
   if (occurrences.empty())
   {
      return {};
   }

   std::sort(candidates.begin(), candidates.end(), [](Occurrence const &a, Occurrence const &b) { return a.hasNarrowerScopeThan(b); });
   return candidates[0];
}

Occurrence const &Topic::nextOccurrenceAfter(Identifier reference) const
{
   auto it = occurrences.find(reference);
   if (it == occurrences.end())
   {
      throw std::runtime_error("unknown occurrence requested");
   }
   it++;
   return (it != occurrences.end()) ? *it->second : *occurrences.begin()->second;
}

Occurrence const &Topic::previousOccurrenceBefore(Identifier reference) const
{
   auto it = occurrences.find(reference);
   if (it == occurrences.end())
   {
      throw std::runtime_error("unknown occurrence requested");
   }
   if (it == occurrences.begin())
   {
      return *occurrences.rbegin()->second;
   }
   it--;
   return *it->second;
}

std::optional<std::reference_wrapper<Occurrence const>> Topic::getOccurrence(contomap::model::Identifier occurrenceId) const
{
   auto it = occurrences.find(occurrenceId);
   return (it != occurrences.end()) ? std::make_optional<std::reference_wrapper<Occurrence const>>(*it->second)
                                    : std::optional<std::reference_wrapper<Occurrence const>> {};
}

Search<Occurrence const> Topic::findOccurrences(Identifiers const &ids) const // NOLINT
{
   for (auto const &[occurrenceId, occurrence] : occurrences)
   {
      if (ids.contains(occurrenceId))
      {
         co_yield *occurrence;
      }
   }
}

Search<Occurrence> Topic::findOccurrences(Identifiers const &ids) // NOLINT
{
   for (auto &[occurrenceId, occurrence] : occurrences)
   {
      if (ids.contains(occurrenceId))
      {
         co_yield *occurrence;
      }
   }
}

Search<Role const> Topic::rolesAssociatedWith(Identifiers associations) const // NOLINT
{
   for (auto const &[_, entry] : roles)
   {
      if (associations.contains(entry->role().getParent()))
      {
         co_yield entry->role();
      }
   }
}

Search<Role const> Topic::findRoles(contomap::model::Identifiers const &ids) const // NOLINT
{
   for (auto const &[roleId, entry] : roles)
   {
      if (ids.contains(roleId))
      {
         co_yield entry->role();
      }
   }
}

Search<Role> Topic::findRoles(contomap::model::Identifiers const &ids) // NOLINT
{
   for (auto &[roleId, entry] : roles)
   {
      if (ids.contains(roleId))
      {
         co_yield entry->role();
      }
   }
}

void Topic::removeTopicReferences(Identifier topicId)
{
   std::erase_if(occurrences, [&topicId](auto const &kvp) {
      auto const &[_, occurrence] = kvp;
      return occurrence->scopeContains(topicId);
   });
   std::erase_if(names, [&topicId](auto const &kvp) {
      auto const &[_, name] = kvp;
      return name.scopeContains(topicId);
   });
   for (auto &[_, occurrence] : occurrences)
   {
      auto typeId = occurrence->getType();
      if (typeId.isAssigned() && (typeId.value() == topicId))
      {
         occurrence->clearType();
      }
   }
   for (auto &[_, entry] : roles)
   {
      auto &role = entry->role();
      auto typeId = role.getType();
      if (typeId.isAssigned() && (typeId.value() == topicId))
      {
         role.clearType();
      }
   }
}

std::optional<std::reference_wrapper<TopicName>> Topic::findNameByScope(Identifiers const &scope)
{
   for (auto &[_, name] : names)
   {
      if (name.scopeEquals(scope))
      {
         return { name };
      }
   }
   return {};
}

void Topic::setReified(Reified &item)
{
   clearReified();
   reified = item;
}

void Topic::clearReified()
{
   if (!reified.has_value())
   {
      return;
   }
   Reified &old = reified.value();
   reified.reset();
   old.clearReifier();
}
