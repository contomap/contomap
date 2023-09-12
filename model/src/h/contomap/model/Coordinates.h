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
