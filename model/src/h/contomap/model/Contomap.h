#pragma once

#include <map>

#include "contomap/model/Association.h"
#include "contomap/model/ContomapView.h"
#include "contomap/model/Identifier.h"
#include "contomap/model/Style.h"
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

   /**
    * @return the identifier of the default topic scope.
    */
   [[nodiscard]] contomap::model::Identifier getDefaultScope() const;

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
    * Deletes the associations with given identifiers.
    *
    * @param ids the identifiers of all associations to remove
    */
   void deleteAssociations(contomap::model::Identifiers const &ids);

   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Topic const> find(std::shared_ptr<Filter<contomap::model::Topic>> filter) const override;
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Topic> find(std::shared_ptr<Filter<contomap::model::Topic>> filter);
   [[nodiscard]] std::optional<std::reference_wrapper<contomap::model::Topic const>> findTopic(contomap::model::Identifier id) const override;
   [[nodiscard]] contomap::infrastructure::Search<contomap::model::Association const> find(
      std::shared_ptr<Filter<contomap::model::Association>> filter) const override;
   [[nodiscard]] std::optional<std::reference_wrapper<contomap::model::Association const>> findAssociation(contomap::model::Identifier id) const override;
   [[nodiscard]] std::optional<std::reference_wrapper<contomap::model::Association>> findAssociation(contomap::model::Identifier id);

private:
   Contomap();

   void deleteAssociation(contomap::model::Identifier id);

   std::map<contomap::model::Identifier, contomap::model::Topic> topics;
   std::map<contomap::model::Identifier, contomap::model::Association> associations;
   std::map<contomap::model::Identifier, contomap::model::Style> styles;
   contomap::model::Identifier defaultScope;
};

}
