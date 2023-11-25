#pragma once

namespace contomap::model
{

class Reified;

/**
 * A Reifier is something that describes something else.
 *
 * @tparam T the type this reifier represents.
 */
template <typename T> class Reifier
{
public:
   /**
    * @return the refined reference to this instance.
    */
   virtual T &refine() = 0;

   /**
    * Declares this instance to be the reifier of another item.
    * If this instance is currently the reifier of another item, the association with the other item will be removed first.
    *
    * @param item the reference of the other item.
    */
   virtual void setReified(Reified &item) = 0;

   /**
    * Clears the reification state of this instance.
    */
   virtual void clearReified() = 0;

protected:
   virtual ~Reifier() = default;
};

}
