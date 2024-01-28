#pragma once

#include <algorithm>

#include "contomap/infrastructure/Generator.h"
#include "contomap/infrastructure/Link.h"
#include "contomap/infrastructure/Referable.h"

namespace contomap::infrastructure
{

/**
 * Sized describes a container of references.
 */
class Sized
{
public:
   /**
    * Requests to clear the container of all references.
    */
   virtual void clear() = 0;

   /**
    * @return the number of contained references.
    */
   [[nodiscard]] virtual size_t size() const = 0;

protected:
   virtual ~Sized() = default;
};

/**
 * LinkedReferences is a set of referable entities.
 *
 * @tparam T the type of the referables.
 */
template <class T> class LinkedReferences : public Sized
{
public:
   /**
    * Default constructor.
    */
   LinkedReferences() = default;
   /**
    * Copy constructor.
    *
    * @param other the other references to add.
    */
   LinkedReferences(LinkedReferences<T> const &other)
   {
      assignFrom(other);
   }
   /**
    * Default move constructor.
    */
   LinkedReferences(LinkedReferences<T> &&) = default;
   /**
    * Constructor with iterator initialization.
    *
    * @tparam Iterator the iterator type
    * @param begin start of the range.
    * @param end end of the range.
    */
   template <class Iterator> LinkedReferences(Iterator begin, Iterator end)
   {
      while (begin != end)
      {
         add(*begin);
         ++begin;
      }
   }
   ~LinkedReferences() override = default;

   /**
    * Copy operator.
    *
    * @param other the other references to add.
    * @return this.
    */
   LinkedReferences &operator=(LinkedReferences<T> const &other)
   {
      assignFrom(other);
      return *this;
   }
   /**
    * Default move operator.
    * @return this.
    */
   LinkedReferences &operator=(LinkedReferences<T> &&) = default;

   /**
    * Clears the set to be empty.
    */
   void clear() override
   {
      references.clear();
   }

   /**
    * Adds the given referable to the collection. If the collection already contains the given referable,
    * no additional link will be established.
    *
    * @param referable the referable to add.
    */
   void add(contomap::infrastructure::Referable<T> &referable)
   {
      if (contains(referable))
      {
         return;
      }
      auto localLinkEntry = references.insert(references.end(), std::unique_ptr<Link<T>> {});
      auto link = referable.linkReference([this, localLinkEntry]() { references.erase(localLinkEntry); });
      *localLinkEntry = std::move(link);
   }

   /**
    * Removes the given referable from the collection. If the collection does not contain the given referable,
    * nothing happens.
    *
    * @param referable the referable to remove.
    * @returns true if the given referable was contained before.
    */
   bool remove(contomap::infrastructure::Referable<T> const &referable)
   {
      return std::erase_if(references, [&referable](auto const &entry) { return &entry->getLinked() == &referable; }) > 0;
   }

   /**
    * @return a generator for all currently contained references.
    */
   [[nodiscard]] contomap::infrastructure::Search<T> allReferences() const // NOLINT
   {
      for (auto const &reference : references)
      {
         co_yield reference->getLinked();
      }
   }

   /**
    * @return true if the collection is empty.
    */
   [[nodiscard]] bool empty() const
   {
      return references.empty();
   }

   [[nodiscard]] size_t size() const override
   {
      return references.size();
   }

   /**
    * Tests whether a given referable is within the collection.
    *
    * @param referable the referable to look for.
    * @return true if the given referable is currently part of the collection, false otherwise.
    */
   [[nodiscard]] bool contains(contomap::infrastructure::Referable<T> const &referable) const
   {
      return std::any_of(references.begin(), references.end(), [&referable](auto const &reference) { return &reference->getLinked() == &referable; });
   }

   /**
    * Tests whether the given set of references are all contained within this collection.
    *
    * @param other the other set of references to test.
    * @return true if all the given references are contained within this collection, false otherwise.
    */
   [[nodiscard]] bool contains(LinkedReferences<T> const &other) const
   {
      return std::all_of(
         other.references.begin(), other.references.end(), [this](auto const &otherLinkEntry) { return this->contains(otherLinkEntry->getLinked()); });
   }

private:
   void assignFrom(LinkedReferences<T> const &other)
   {
      references.clear();
      for (auto const &otherLinkEntry : other.references)
      {
         add(otherLinkEntry->getLinked());
      }
   }

   std::list<std::unique_ptr<contomap::infrastructure::Link<T>>> references;
};

}
