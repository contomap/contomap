#pragma once

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
    * @param value the new spacial coordinate
    */
   void setSpacial(contomap::model::SpacialCoordinate value);
   /**
    * @return the current spacial coordinate
    */
   [[nodiscard]] contomap::model::SpacialCoordinate getSpacial() const;

private:
   contomap::model::SpacialCoordinate spacial;
};

}
