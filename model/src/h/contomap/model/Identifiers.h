#pragma once

#include <ostream>
#include <set>

#include "contomap/model/Identifier.h"

namespace contomap::model
{

/**
 * Identifiers is a set of Identifier values.
 */
class Identifiers
{
public:
   /** Internal type used for storing the identifiers. */
   using CollectionType = std::set<Identifier>;

   /**
    * Equality operator.
    * @param other the other identifiers to compare against.
    * @return true if equal.
    */
   bool operator==(Identifiers const &other) const = default;
   /**
    * Inequality operator.
    * @param other the other identifiers to compare against.
    * @return true if not equal.
    */
   bool operator!=(Identifiers const &other) const = default;

   /**
    * @return start operator for iteration.
    */
   [[nodiscard]] CollectionType::const_iterator begin() const;
   /**
    * @return stop operator for iteration.
    */
   [[nodiscard]] CollectionType::const_iterator end() const;

   /**
    * Adds the given identifier to the collection. If the identifier is already contained, nothing happens.
    *
    * @param id the identifier to add.
    */
   void add(contomap::model::Identifier id);
   /**
    * Removes the given identifier from the collection. If the identifier was not contained, nothing happens.
    *
    * @param id the identifier to remove.
    */
   void remove(contomap::model::Identifier id);

   /**
    * @return true if the collection is empty, false otherwise.
    */
   [[nodiscard]] bool empty() const;
   /**
    * Tests whether the given identifier is contained in this collection.
    *
    * @param id the identifier to check for.
    * @return true if the identifier is contained, false otherwise.
    */
   [[nodiscard]] bool contains(Identifier id) const;
   /**
    * Tests whether the given collection is contained in this collection.
    *
    * @param other the other collection to check for.
    * @return true if all the provided identifier are contained.
    */
   [[nodiscard]] bool contains(Identifiers const &other) const;

   /**
    * Write the given collection to the given stream.
    *
    * @param stream the stream to write to
    * @param identifiers the collection to write
    * @return the provided stream
    */
   friend std::ostream &operator<<(std::ostream &stream, Identifiers const &identifiers)
   {
      bool first = true;
      stream << '[';
      for (auto const &id : identifiers)
      {
         if (!first)
         {
            stream << ", ";
         }
         first = false;
         stream << id;
      }
      stream << ']';
      return stream;
   }

private:
   CollectionType set;
};

}
