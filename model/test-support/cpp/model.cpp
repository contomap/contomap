#include "contomap/test/printers/model.h"

namespace contomap::model
{

std::ostream &operator<<(std::ostream &stream, contomap::model::SpacialCoordinate const &spacial)
{
   auto absoluteReference = spacial.getAbsoluteReference();
   stream << "absoluteReference={";
   stream << "X=" << absoluteReference.X() << ", ";
   stream << "Y=" << absoluteReference.Y();
   stream << "}";
   return stream;
}

}
