#include "contomap/model/Topic.h"

using contomap::infrastructure::Link;
using contomap::infrastructure::Links;
using contomap::infrastructure::Search;
using contomap::infrastructure::serial::Coder;
using contomap::infrastructure::serial::Decoder;
using contomap::infrastructure::serial::Encoder;
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

void Topic::encodeProperties(Encoder &encoder) const
{
   Coder::Scope scope(encoder, "properties");
   encoder.codeArray("names", names.begin(), names.end(), [](Encoder &nested, auto const &kvp) {
      Coder::Scope nameScope(nested, "");
      kvp.first.encode(nested, "id");
      kvp.second.encode(nested);
   });
}

void Topic::decodeProperties(Decoder &decoder, uint8_t version)
{
   Coder::Scope scope(decoder, "properties");
   decoder.codeArray("names", [this, version](Decoder &nested, size_t) {
      Coder::Scope nameScope(nested, "");
      auto nameId = Identifier::from(nested, "id");
      auto name = TopicName::from(nested, version, nameId);
      names.emplace(nameId, name);
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
   auto it = occurrences.emplace(occurrenceId, std::make_unique<Occurrence>(occurrenceId, id, std::move(scope), location));
   return *it.first->second;
}

bool Topic::removeOccurrence(Identifier occurrenceId)
{
   return occurrences.erase(occurrenceId) > 0;
}

Role &Topic::newRole(Association &association)
{
   auto const &seed = association.addRole();
   auto role = std::make_unique<Role>(seed.getId(), *this, association);
   auto it = roles.find(seed.getId());
   it->second->role = std::move(role);
   return *it->second->role;
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
   for (auto it = roles.begin(); it != roles.end();)
   {
      if (association.removeRole(*it->second->role))
      {
         it = roles.erase(it);
      }
      else
      {
         ++it;
      }
   }
}

void Topic::removeRole(Association &association, Identifier roleId)
{
   if (!roles.contains(roleId))
   {
      return;
   }
   association.removeRole(*roles.at(roleId)->role);
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
   for (auto const &[_, role] : roles)
   {
      if (associations.contains(role->role->getParent()))
      {
         co_yield *role->role;
      }
   }
}

Search<Role const> Topic::findRoles(contomap::model::Identifiers const &ids) const // NOLINT
{
   for (auto const &[roleId, role] : roles)
   {
      if (ids.contains(roleId))
      {
         co_yield *role->role;
      }
   }
}

Search<Role> Topic::findRoles(contomap::model::Identifiers const &ids) // NOLINT
{
   for (auto &[roleId, role] : roles)
   {
      if (ids.contains(roleId))
      {
         co_yield *role->role;
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
   for (auto &[_, role] : roles)
   {
      auto typeId = role->role->getType();
      if (typeId.isAssigned() && (typeId.value() == topicId))
      {
         role->role->clearType();
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
