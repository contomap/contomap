#pragma once

#include <map>

#include "contomap/model/Association.h"
#include "contomap/model/ContomapView.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Topic.h"

namespace contomap::model
{

/**
 * Contomap is the overall type to contain all the information.
 */
class Contomap : public contomap::model::ContomapView
{
public:
   /**
    * @return a new map instance with a default view scope.
    */
   static Contomap newMap();

   [[nodiscard]] contomap::model::Identifier getDefaultScope() const override;

   /**
    * Creates a new topic and adds it to the map.
    *
    * @return the reference to the created instance.
    */
   [[nodiscard]] contomap::model::Topic &newTopic();

   /**
    * Crates a new association and adds it to the map.
    *
    * @param scope the scope within which the association is valid.
    * @param location the location in the graphical map.
    * @return the created instance.
    */
   [[nodiscard]] contomap::model::Association &newAssociation(contomap::model::Identifiers scope, contomap::model::SpacialCoordinate location);

   /**
    * Deletes the roles with given identifiers.
    *
    * @param ids the identifiers of all roles to remove.
    */
   void deleteRoles(contomap::model::Identifiers const &ids);

   /**
    * Deletes the associations with given identifiers.
    *
    * @param ids the identifiers of all associations to remove.
    */
   void deleteAssociations(contomap::model::Identifiers const &ids);

   /**
    * Deletes the occurrences with given identifiers.
    *
    * @param ids the identifiers of all occurrences to remove.
    */
   void deleteOccurrences(contomap::model::Identifiers const &ids);

   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Topic const> find(
      std::shared_ptr<contomap::model::Filter<contomap::model::Topic>> filter) const override;

   /**
    * Find topics that match a certain filter, for potential modification.
    *
    * The returned search object will yield all topics for which the filter returns true.
    *
    * @param filter the filter to call.
    * @return a Search instance that can be iterated once.
    */
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Topic> find(std::shared_ptr<contomap::model::Filter<contomap::model::Topic>> filter);
   [[nodiscard]] std::optional<std::reference_wrapper<contomap::model::Topic const>> findTopic(contomap::model::Identifier id) const override;
   /**
    * Find a topic with a specific identifier, for potential modification.
    *
    * @param id the identifier to search for.
    * @return a reference to the matching topic, if existing.
    */
   [[nodiscard]] std::optional<std::reference_wrapper<contomap::model::Topic>> findTopic(contomap::model::Identifier id);
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Association const> find(
      std::shared_ptr<contomap::model::Filter<contomap::model::Association>> filter) const override;
   [[nodiscard]] std::optional<std::reference_wrapper<contomap::model::Association const>> findAssociation(contomap::model::Identifier id) const override;

   /**
    * Find a association with a specific identifier, for potential modification.
    *
    * @param id the identifier to search for.
    * @return a reference to the matching association, if existing.
    */
   [[nodiscard]] std::optional<std::reference_wrapper<contomap::model::Association>> findAssociation(contomap::model::Identifier id);

   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Occurrence const> findOccurrences(contomap::model::Identifiers const &ids) const override;
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Occurrence> findOccurrences(contomap::model::Identifiers const &ids);
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Role const> findRoles(contomap::model::Identifiers const &ids) const override;
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Role> findRoles(contomap::model::Identifiers const &ids);

private:
   Contomap();

   void deleteRole(contomap::model::Identifier id);
   void deleteAssociation(contomap::model::Identifier id);
   void deleteOccurrence(contomap::model::Identifier id);
   void deleteTopicsCascading(contomap::model::Identifiers toDelete);
   bool topicShouldBeRemoved(Topic const &topic);
   void deleting(contomap::model::Identifiers &toDelete, contomap::model::Topic &topic);

   std::map<contomap::model::Identifier, contomap::model::Topic> topics;
   std::map<contomap::model::Identifier, contomap::model::Association> associations;
   contomap::model::Identifier defaultScope;
};

}
