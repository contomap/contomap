#include <gmock/gmock.h>

#include "ViewScopeFixture.h"

using contomap::model::Identifier;
using contomap::model::Identifiers;

using contomap::test::fixtures::ViewScopeFixture;

ViewScopeFixture::ViewScopeFixture(Identifiers const &scope)
   : scope(scope)
{
}

void ViewScopeFixture::shouldContain(Identifier id) const
{
   EXPECT_TRUE(scope.contains(id)) << scope;
}

void ViewScopeFixture::shouldNotContain(Identifier id) const
{
   EXPECT_FALSE(scope.contains(id)) << scope;
}

void ViewScopeFixture::shouldBe(Identifiers expected) const
{
   EXPECT_EQ(expected, scope);
}
