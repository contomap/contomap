#include "contomap/model/Topic.h"

using contomap::infrastructure::Search;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::Role;
using contomap::model::SpacialCoordinate;
using contomap::model::Topic;
using contomap::model::TopicName;

Topic::Topic(Identifier id)
   : id(id)
{
}

Identifier Topic::getId() const
{
   return id;
}

TopicName &Topic::newName(contomap::model::TopicNameValue value)
{
   auto nameId = Identifier::random();
   auto it = names.emplace(nameId, TopicName(nameId, std::move(value)));
   return it.first->second;
}

Search<TopicName const> Topic::allNames() const // NOLINT
{
   for (auto const &[_, name] : names)
   {
      co_yield name;
   }
}

Occurrence &Topic::newOccurrence(Identifiers scope, SpacialCoordinate location)
{
   auto occurrenceId = Identifier::random();
   auto it = occurrences.emplace(occurrenceId, Occurrence(occurrenceId, std::move(scope), location));
   return it.first->second;
}

bool Topic::removeOccurrence(Identifier occurrenceId)
{
   return occurrences.erase(occurrenceId) > 0;
}

Role &Topic::newRole(Association &association)
{
   auto const &seed = association.addRole();
   auto it = roles.emplace(seed.getId(), seed);
   return it.first->second;
}

void Topic::removeRolesOf(Association &association)
{
   for (auto it = roles.begin(); it != roles.end();)
   {
      if (association.removeRole(it->second))
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
   association.removeRole(roles.at(roleId));
   roles.erase(roleId);
}

bool Topic::isIn(Identifiers const &scope) const
{
   return std::any_of(occurrences.begin(), occurrences.end(), [&scope](std::pair<Identifier, Occurrence> const &kvp) { return kvp.second.isIn(scope); });
}

bool Topic::occursAsAnyOf(Identifiers const &occurrenceIds) const
{
   return std::any_of(
      occurrences.begin(), occurrences.end(), [&occurrenceIds](std::pair<Identifier, Occurrence> const &kvp) { return occurrenceIds.contains(kvp.first); });
}

bool Topic::isWithoutOccurrences() const
{
   return occurrences.empty();
}

Search<Occurrence const> Topic::occurrencesIn(contomap::model::Identifiers scope) const // NOLINT
{
   for (auto const &[_, occurrence] : occurrences)
   {
      if (occurrence.isIn(scope))
      {
         co_yield occurrence;
      }
   }
}

Occurrence const &Topic::nextOccurrenceAfter(Identifier reference) const
{
   auto it = occurrences.find(reference);
   if (it == occurrences.end())
   {
      throw std::runtime_error("unknown occurrence requested");
   }
   it++;
   return (it != occurrences.end()) ? it->second : occurrences.begin()->second;
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
      return occurrences.rbegin()->second;
   }
   it--;
   return it->second;
}

std::optional<std::reference_wrapper<Occurrence const>> Topic::getOccurrence(contomap::model::Identifier occurrenceId) const
{
   auto it = occurrences.find(occurrenceId);
   return (it != occurrences.end()) ? std::make_optional<std::reference_wrapper<Occurrence const>>(it->second)
                                    : std::optional<std::reference_wrapper<Occurrence const>> {};
}

Search<Role const> Topic::rolesAssociatedWith(Identifiers associations) const // NOLINT
{
   for (auto const &[_, role] : roles)
   {
      if (associations.contains(role.getParent()))
      {
         co_yield role;
      }
   }
}

void Topic::removeTopicReferences(Identifier topicId)
{
   for (auto it = occurrences.begin(); it != occurrences.end();)
   {
      if (it->second.scopeContains(topicId))
      {
         it = occurrences.erase(it);
      }
      else
      {
         ++it;
      }
   }
}
