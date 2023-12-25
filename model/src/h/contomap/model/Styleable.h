#pragma once

#include "contomap/model/Style.h"

namespace contomap::model
{

/**
 * A Styleable is something that can be styled in its appearance.
 */
class Styleable
{
public:
   /**
    * Set the style of the appearance.
    *
    * @param style the new style to set.
    */
   virtual void setAppearance(contomap::model::Style style) = 0;
   /**
    * @return the current style of the appearance.
    */
   [[nodiscard]] virtual contomap::model::Style getAppearance() const = 0;

protected:
   virtual ~Styleable() = default;
};

}
