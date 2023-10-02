#pragma once

#include "contomap/model/ContomapView.h"

namespace contomap::test::fixtures
{

class ContomapViewFixture
{
public:
   explicit ContomapViewFixture(contomap::model::ContomapView const &view);

   void shouldHaveTopicCountOf(size_t expected);
   void shouldHaveTopicThat(contomap::model::Identifier id, std::function<void(contomap::model::Topic const &)> const &asserter);
   void shouldHaveAssociationThat(contomap::model::Identifier id, std::function<void(contomap::model::Association const &)> const &asserter);

   void shouldHaveOneAssociationNear(contomap::model::SpacialCoordinate position);

private:
   contomap::model::ContomapView const &view;
};

}