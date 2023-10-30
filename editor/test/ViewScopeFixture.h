#pragma once

#include "contomap/model/Identifiers.h"

namespace contomap::test::fixtures
{

class ViewScopeFixture
{
public:
   explicit ViewScopeFixture(contomap::model::Identifiers const &scope);

   void shouldContain(contomap::model::Identifier id) const;
   void shouldNotContain(contomap::model::Identifier id) const;
   void shouldBe(contomap::model::Identifiers expected) const;

private:
   contomap::model::Identifiers const &scope;
};

}