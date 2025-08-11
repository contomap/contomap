#pragma once

#include <list>

#include "contomap/infrastructure/Link.h"

namespace contomap::infrastructure
{

/**
 * Referable describes something that can be kept referenced by LinkedReferences and other uni-directional
 * entities. Others can refer to this instance, yet a Referable doesn't know the details about its references.
 *
 * @tparam T the concrete type of the referable.
 */
template <class T> class Referable
{
public:
   /**
    * @return the refined referable to this instance.
    */
   virtual T &refine() = 0;

   /**
    * Creates a link to this referable.
    *
    * @param referableUnlinked the function to pass on to the returned link.
    * @return the link that refers to this instance.
    */
   std::unique_ptr<contomap::infrastructure::Link<T>> linkReference(std::function<void()> referableUnlinked)
   {
      static Reference sentinel;
      auto localLinkEntry = references.insert(references.end(), std::unique_ptr<Link<Reference>> {});
      auto links = Links::between(refine(), std::move(referableUnlinked), sentinel, [this, localLinkEntry]() { references.erase(localLinkEntry); });
      *localLinkEntry = std::move(links.second);
      return std::move(links.first);
   }

protected:
   virtual ~Referable() = default;

private:
   struct Reference
   {
   };
   std::list<std::unique_ptr<contomap::infrastructure::Link<Reference>>> references;
};

}
