#pragma once

#include <ostream>
#include <set>

#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
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
    * Factory function to create an instance with a single entry.
    *
    * @param id the identifier to contain
    * @return a new instance
    */
   static Identifiers ofSingle(Identifier id);

   /**
    * Spaceship operator.
    *
    * @param other the other instance to compare to.
    * @return the ordering for this type.
    */
   std::strong_ordering operator<=>(Identifiers const &other) const noexcept = default;

   /**
    * @return start operator for iteration.
    */
   [[nodiscard]] CollectionType::const_iterator begin() const;
   /**
    * @return stop operator for iteration.
    */
   [[nodiscard]] CollectionType::const_iterator end() const;

   /**
    * @return the number of identifiers in the set.
    */
   [[nodiscard]] size_t size() const;

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
    * @return true if the given identifier was in that collection.
    */
   bool remove(contomap::model::Identifier id);

   /**
    * Removes all identifiers from the collection.
    */
   void clear();

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
    * Serializes the identifiers with given coder.
    *
    * @param encoder the encoder to use.
    * @param name the name to specify for the entry.
    */
   void encode(contomap::infrastructure::serial::Encoder &encoder, std::string const &name) const;
   /**
    * Deserializes the identifiers with given coder.
    *
    * @param decoder the encoder to use.
    * @param name the name to specify for the entry.
    */
   void decode(contomap::infrastructure::serial::Decoder &decoder, std::string const &name);

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
