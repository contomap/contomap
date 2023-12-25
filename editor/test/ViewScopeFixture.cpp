#include <gmock/gmock.h>

#include "ViewScopeFixture.h"

using contomap::editor::ViewScope;
using contomap::model::Identifier;
using contomap::model::Identifiers;

using contomap::test::fixtures::ViewScopeFixture;

ViewScopeFixture::ViewScopeFixture(ViewScope const &viewScope)
   : viewScope(viewScope)
{
}

void ViewScopeFixture::shouldContain(Identifier id) const
{
   Identifiers scope = viewScope.identifiers();
   EXPECT_TRUE(scope.contains(id)) << scope;
}

void ViewScopeFixture::shouldNotContain(Identifier id) const
{
   Identifiers scope = viewScope.identifiers();
   EXPECT_FALSE(scope.contains(id)) << scope;
}

void ViewScopeFixture::shouldBe(Identifiers const &expected) const
{
   Identifiers scope = viewScope.identifiers();
   EXPECT_EQ(expected, scope);
}
