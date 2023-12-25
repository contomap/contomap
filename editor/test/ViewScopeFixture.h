#pragma once

#include "contomap/editor/ViewScope.h"

namespace contomap::test::fixtures
{

class ViewScopeFixture
{
public:
   explicit ViewScopeFixture(contomap::editor::ViewScope const &viewScope);

   void shouldContain(contomap::model::Identifier id) const;
   void shouldNotContain(contomap::model::Identifier id) const;
   void shouldBe(contomap::model::Identifiers const &expected) const;

private:
   contomap::editor::ViewScope const &viewScope;
};

}