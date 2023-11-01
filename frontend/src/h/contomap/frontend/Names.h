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
    * @param defaultScope the topic identifier that refers to the default scope.
    * @return a list of strings, ordered by priority. Always contains at least one element.
    */
   static std::vector<std::string> forDisplay(contomap::model::Topic const &topic, contomap::model::Identifier defaultScope);
};

} // namespace contomap::frontend
