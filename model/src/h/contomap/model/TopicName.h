#pragma once

#include "contomap/model/Identifier.h"
#include "contomap/model/Identifiers.h"
#include "contomap/model/OptionalIdentifier.h"
#include "contomap/model/TopicNameValue.h"

namespace contomap::model
{

/**
 * A TopicName represents the human interpretable identification of a topic.
 */
class TopicName
{
public:
   /**
    * Constructor.
    *
    * @param id the primary identifier of this name.
    * @param scope the scope within which this name is valid.
    * @param value the human readable name.
    */
   TopicName(contomap::model::Identifier id, contomap::model::Identifiers scope, contomap::model::TopicNameValue value);

   /**
    * @return the current value of this name.
    */
   [[nodiscard]] contomap::model::TopicNameValue getValue() const;

   /**
    * Return true if this instance is in the given scope.
    *
    * @param thatScope the scope to look for.
    * @return true if the name is in given scope.
    */
   [[nodiscard]] bool isIn(contomap::model::Identifiers const &thatScope) const;

   /**
    * Return true if the scope contains the given identifier.
    *
    * @param thatId the identifier to check .
    * @return true in case the identifier is part of the scope.
    */
   [[nodiscard]] bool scopeContains(contomap::model::Identifier thatId) const;

   /**
    * Use this method to sort names according to scope.
    *
    * @param other the other name to compare to.
    * @return true if this name has a narrower (longer) scope than the other. Also returns true if same size but smaller ID values.
    */
   [[nodiscard]] bool hasNarrowerScopeThan(TopicName const &other) const;
   /**
    * Use this method to determine whether a name is equivalent when considering scopes.
    *
    * @param other the other name to compare to.
    * @return true if this and the other name have the same size of their scopes.
    */
   [[nodiscard]] bool hasSameScopeSizeAs(TopicName const &other) const;

private:
   contomap::model::Identifier id;
   contomap::model::Identifiers scope;

   contomap::model::OptionalIdentifier reifier;

   contomap::model::TopicNameValue value;
};

}
