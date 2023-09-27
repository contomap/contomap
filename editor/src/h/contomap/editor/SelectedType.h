#pragma once

namespace contomap::editor
{

/**
 * SelectedType specifies what kind of a thing a selection identifier refers to.
 */
enum class SelectedType
{
   /**
    * An occurrence is a representation of a topic in a scope.
    */
   Occurrence,
   /**
    * An association is a link between topics.
    */
   Association,
   /**
    * A role is the link of a topic to an association.
    */
   Role,
};

} // namespace contomap::frontend
