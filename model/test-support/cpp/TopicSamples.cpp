#include "contomap/test/samples/TopicSamples.h"

using contomap::model::Identifier;
using contomap::model::Topic;

Topic contomap::test::samples::someTopic()
{
   return Topic(Identifier::random());
}
