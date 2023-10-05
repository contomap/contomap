#include "contomap/test/samples/ScopeSamples.h"

using contomap::model::Identifier;
using contomap::model::Identifiers;

namespace contomap::test::samples
{

Identifiers someNonEmptyScope()
{
   Identifiers scope;
   scope.add(Identifier::random());
   return scope;
}

}
