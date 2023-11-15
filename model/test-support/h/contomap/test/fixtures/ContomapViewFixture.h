#pragma once

#include "contomap/model/ContomapView.h"

namespace contomap::test::fixtures
{

class ContomapViewFixture
{
public:
   explicit ContomapViewFixture(contomap::model::ContomapView const &view);

   void shouldHaveTopicCountOf(size_t expected) const;
   void shouldNotHaveTopic(contomap::model::Identifier id) const;
   void shouldHaveTopicThat(contomap::model::Identifier id, std::function<void(contomap::model::Topic const &)> const &asserter) const;
   void shouldHaveAssociationCountOf(size_t expected) const;
   void shouldNotHaveAssociation(contomap::model::Identifier id) const;
   void shouldHaveAssociationThat(contomap::model::Identifier id, std::function<void(contomap::model::Association const &)> const &asserter) const;

   void shouldHaveOneAssociationNear(contomap::model::SpacialCoordinate position) const;

private:
   contomap::model::ContomapView const &view;
};

}