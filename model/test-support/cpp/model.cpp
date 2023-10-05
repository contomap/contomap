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

std::ostream &operator<<(std::ostream &stream, contomap::model::Topic const &topic)
{
   stream << "id=" << topic.getId();
   stream << ", names=[";
   for (contomap::model::TopicName const &name : topic.allNames())
   {
      stream << " \"" << name.getValue().raw() << '"';
   }
   stream << " ]";
   return stream;
}

}
