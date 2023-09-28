#include <gmock/gmock.h>

#include "contomap/model/Association.h"

#include "contomap/test/matchers/Coordinates.h"
#include "contomap/test/printers/model.h"
#include "contomap/test/samples/CoordinateSamples.h"
#include "contomap/test/samples/ScopeSamples.h"

using contomap::model::Association;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::SpacialCoordinate;

using contomap::test::matchers::isCloseTo;
using contomap::test::samples::someNonEmptyScope;
using contomap::test::samples::someSpacialCoordinate;

TEST(AssociationTest, baseProperties)
{
   auto id = Identifier::random();
   Identifiers scope = someNonEmptyScope();
   auto position = someSpacialCoordinate();
   Association a(id, scope, position);
   EXPECT_EQ(id, a.getId());
   EXPECT_TRUE(a.isIn(scope));
   EXPECT_FALSE(a.isIn({}));
   EXPECT_THAT(a.getLocation().getSpacial(), isCloseTo(position));
}
