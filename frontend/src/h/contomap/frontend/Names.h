#pragma once

#include "contomap/model/Identifiers.h"
#include "contomap/model/Topic.h"

namespace contomap::frontend
{

/**
 * Names provides some lookup logic for topic names.
 */
class Names
{
public:
   /**
    * Return a list of strings that are meant for display.
    *
    * @param topic the topic for which to determine the name
    * @param defaultScope the topic that refers to the default scope.
    * @return a list of strings, ordered by priority. Always contains at least one element.
    */
   static std::vector<std::string> forDisplay(contomap::model::Topic const &topic, contomap::model::Topic const &defaultScope);

   /**
    * Return a strings that is the best candidate for a scoped display.
    *
    * @param topic the topic for which to determine the name
    * @param scope the current scope to consider.
    * @param defaultScope the topic that refers to the default scope.
    * @return a list of strings, ordered by priority. Always contains at least one element.
    */
   static std::vector<std::string> forScopedDisplay(
      contomap::model::Topic const &topic, contomap::model::Identifiers const &scope, contomap::model::Topic const &defaultScope);
};

} // namespace contomap::frontend
