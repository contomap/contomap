#pragma once

#include "contomap/infrastructure/serial/Decoder.h"
#include "contomap/infrastructure/serial/Encoder.h"
#include "contomap/model/SpacialCoordinate.h"

namespace contomap::model
{

/**
 * Coordinate describes when and where a map element shall be shown.
 */
class Coordinates
{
public:
   /**
    * Default constructor.
    */
   Coordinates() = default;
   /**
    * Constructor for initial spacial coordinates.
    *
    * @param spacial the initial coordinates
    */
   explicit Coordinates(contomap::model::SpacialCoordinate spacial);

   /**
    * Serializes the coordinates.
    *
    * @param coder the encoder to use.
    * @param name the name to use for the scope.
    */
   void encode(contomap::infrastructure::serial::Encoder &coder, std::string const &name) const;

   /**
    * Deserializes the coordinates.
    *
    * @param coder the decoder to use.
    * @param name the name to use for the scope.
    * @param version the version to consider.
    */
   void decode(contomap::infrastructure::serial::Decoder &coder, std::string const &name, uint8_t version);

   /**
    * @param value the new spacial coordinate
    */
   void setSpacial(contomap::model::SpacialCoordinate value);
   /**
    * @return the current spacial coordinate
    */
   [[nodiscard]] contomap::model::SpacialCoordinate getSpacial() const;

   /**
    * Move the spacial location by given offset.
    *
    * @param offset the offset to apply.
    */
   void moveBy(contomap::model::SpacialCoordinate::Offset offset);

private:
   contomap::model::SpacialCoordinate spacial;
};

}
