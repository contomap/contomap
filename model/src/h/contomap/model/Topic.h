#pragma once

#include <list>
#include <map>
#include <memory>

#include "contomap/infrastructure/Generator.h"
#include "contomap/infrastructure/Link.h"
#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/Association.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Occurrence.h"
#include "contomap/model/Reified.h"
#include "contomap/model/Reifier.h"
#include "contomap/model/Role.h"
#include "contomap/model/TopicName.h"

namespace contomap::model
{

/**
 * A Topic captures the information about a particular subject.
 */
class Topic : public contomap::model::Reifier<Topic>
{
public:
   /**
    * Constructor.
    *
    * @param id the primary identifier of this name.
    */
   explicit Topic(contomap::model::Identifier id);
   ~Topic() override;

   Topic &refine() override;

   /**
    * Serialize the related items of this topic.
    *
    * @param coder the encoder to use.
    */
   void encodeRelated(contomap::infrastructure::serial::Encoder &coder) const;

   /**
    * Deserialize the related items of this topic.
    *
    * @param coder the decoder to use.
    * @param version the version to consider.
    * @param topicResolver the function to use for resolving topic references.
    * @param associationResolver the function to use for resolving association references.
    */
   void decodeRelated(contomap::infrastructure::serial::Decoder &coder, uint8_t version, std::function<Topic &(contomap::model::Identifier)> topicResolver,
      std::function<Association &(contomap::model::Identifier)> associationResolver);

   [[nodiscard]] contomap::model::Identifier getId() const override;

   /**
    * Links the topic to be the type of a typed entity.
    *
    * @param topicUnlinked the function to pass on to the returned link.
    * @return the link that refers to this instance.
    */
   [[nodiscard]] std::unique_ptr<contomap::infrastructure::Link<Topic>> linkTyped(std::function<void()> topicUnlinked);

   /**
    * @return the number of entities this topic is the type of.
    */
   [[nodiscard]] size_t getTypedCount() const;

   /**
    * Adds a new name to the topic.
    *
    * @param scope the scope within which the name is valid.
    * @param value the value of the name to add.
    * @return the created instance.
    */
   [[nodiscard]] contomap::model::TopicName &newName(contomap::model::Identifiers scope, contomap::model::TopicNameValue const &value);

   /**
    * Adds a new occurrence to the topic.
    *
    * @param scope the scope within which the occurrence is valid.
    * @param location the location in the graphical map.
    * @return the created instance.
    */
   [[nodiscard]] contomap::model::Occurrence &newOccurrence(contomap::model::Identifiers scope, contomap::model::SpacialCoordinate location);

   /**
    * Removes any occurrence that has given identifier.
    *
    * @param occurrenceId the identifier of the occurrence to remove.
    * @return true if the topic had the identified occurrence.
    */
   bool removeOccurrence(contomap::model::Identifier occurrenceId);

   /**
    * Adds a new role to the topic.
    *
    * @param association the association this topic has a role in.
    * @return the created instance.
    */
   [[nodiscard]] contomap::model::Role &newRole(contomap::model::Association &association);

   /**
    * Establishes a link with given role.
    *
    * @param role the role instance to link with.
    * @param topicUnlinked the function to pass on to the returned link.
    * @return the link that refers to this instance.
    */
   [[nodiscard]] std::unique_ptr<contomap::infrastructure::Link<Topic>> link(contomap::model::Role &role, std::function<void()> topicUnlinked);

   /**
    * Remove all the roles for given association.
    *
    * @param association the association to no longer relate to.
    */
   void removeRolesOf(contomap::model::Association &association);

   /**
    * Removes the role with given identifier from this topic.
    *
    * @param roleId the role to remove.
    */
   void removeRole(contomap::model::Identifier roleId);

   /**
    * @return a Search for all names in the topic.
    */
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::TopicName const> allNames() const;

   /**
    * Set the name of the topic in given scope.
    *
    * @param scope the scope in which the new name shall be set.
    * @param value the new value.
    */
   void setNameInScope(contomap::model::Identifiers const &scope, contomap::model::TopicNameValue value);

   /**
    * Remove the name within given scope.
    *
    * @param scope the scope from which the name shall be removed.
    */
   void removeNameInScope(contomap::model::Identifiers const &scope);

   /**
    * Return true if this instance has at least one occurrence that is in given scope.
    *
    * @param scope the scope to look for.
    * @return true if the topic is in given scope.
    */
   [[nodiscard]] bool isIn(contomap::model::Identifiers const &scope) const;

   /**
    * Return true if this instance has at least one occurrence that is in given list.
    *
    * @param occurrenceIds the identifiers to look for.
    * @return true if the topic has at least one occurrence in the set.
    */
   [[nodiscard]] bool occursAsAnyOf(contomap::model::Identifiers const &occurrenceIds) const;

   /**
    * @return true if the topic is currently without occurrences.
    */
   [[nodiscard]] bool isWithoutOccurrences() const;

   /**
    * Return a Search for all occurrences that are in given scope.
    *
    * @param scope the scope to look for.
    * @return a Search matching the given scope.
    */
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Occurrence const> occurrencesIn(contomap::model::Identifiers scope) const;

   /**
    * Tries to find an occurrence that ideally is closest to the provided scope.
    *
    * @param scope the scope to look for.
    * @return a reference to the occurrence closest to given scope.
    */
   [[nodiscard]] std::optional<std::reference_wrapper<contomap::model::Occurrence const>> closestOccurrenceTo(contomap::model::Identifiers const &scope) const;

   /**
    * Determines the occurrence that follows the given one.
    *
    * @param reference the identifier of the occurrence to determine.
    * @return reference to an occurrence.
    */
   [[nodiscard]] contomap::model::Occurrence const &nextOccurrenceAfter(contomap::model::Identifier reference) const;

   /**
    * Determines the occurrence that precedes the given one.
    *
    * @param reference the identifier of the occurrence to determine.
    * @return reference to an occurrence.
    */
   [[nodiscard]] contomap::model::Occurrence const &previousOccurrenceBefore(contomap::model::Identifier reference) const;

   /**
    * Resolve the occurrence with given identifier.
    *
    * @param occurrenceId the identifier of the occurrence to retrieve.
    * @return the associated occurrence.
    */
   [[nodiscard]] std::optional<std::reference_wrapper<contomap::model::Occurrence const>> getOccurrence(contomap::model::Identifier occurrenceId) const;

   /**
    * The returned search object will yield all occurrences that are in the set of identifiers.
    *
    * @param ids the set of identifiers to look for.
    * @return a Search instance that can be iterated once.
    */
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Occurrence const> findOccurrences(contomap::model::Identifiers const &ids) const;

   /**
    * The returned search object will yield all occurrences that are in the set of identifiers.
    *
    * @param ids the set of identifiers to look for.
    * @return a Search instance that can be iterated once.
    */
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Occurrence> findOccurrences(contomap::model::Identifiers const &ids);

   /**
    * Return a Search for all roles that are related to given associations.
    *
    * @param associations the associations to look for.
    * @return a Search matching the given scope.
    */
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Role const> rolesAssociatedWith(contomap::model::Identifiers associations) const;

   /**
    * The returned search object will yield all roles that are in the set of identifiers.
    *
    * @param ids the set of identifiers to look for.
    * @return a Search instance that can be iterated once.
    */
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Role const> findRoles(contomap::model::Identifiers const &ids) const;

   /**
    * The returned search object will yield all roles that are in the set of identifiers.
    *
    * @param ids the set of identifiers to look for.
    * @return a Search instance that can be iterated once.
    */
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Role> findRoles(contomap::model::Identifiers const &ids);

   /**
    * Removes any reference to the given identified topic.
    *
    * @param topicId the identifier of the topic that shall no longer be referenced.
    */
   void removeTopicReferences(contomap::model::Identifier topicId);

   void setReified(contomap::model::Reified &item) final;
   void clearReified() final;

private:
   class RoleEntry
   {
   public:
      RoleEntry(std::unique_ptr<contomap::infrastructure::Link<contomap::model::Role>> link)
         : link(std::move(link))
      {
      }

      contomap::model::Role &role()
      {
         return link->getLinked();
      }

      void own(std::unique_ptr<contomap::model::Role> role)
      {
         ownedRole = std::move(role);
      }

   private:
      std::unique_ptr<contomap::model::Role> ownedRole;
      std::unique_ptr<contomap::infrastructure::Link<contomap::model::Role>> link;
   };

   [[nodiscard]] std::optional<std::reference_wrapper<contomap::model::TopicName>> findNameByScope(contomap::model::Identifiers const &scope);

   contomap::model::Identifier id;

   std::list<std::unique_ptr<contomap::infrastructure::Link<std::monostate>>> typed;

   std::map<contomap::model::Identifier, contomap::model::TopicName> names;
   std::map<contomap::model::Identifier, std::unique_ptr<contomap::model::Occurrence>> occurrences;
   std::map<contomap::model::Identifier, std::unique_ptr<RoleEntry>> roles;

   std::optional<std::reference_wrapper<contomap::model::Reified>> reified;
};

}
