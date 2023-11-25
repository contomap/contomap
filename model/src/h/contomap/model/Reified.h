#pragma once

namespace contomap::model
{

/**
 * A Reified is something that is described by something else.
 */
class Reified
{
public:
   /**
    * Clears the reifier of this item.
    */
   virtual void clearReifier() = 0;

protected:
   virtual ~Reified() = default;
};

}
