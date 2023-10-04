#include <gmock/gmock.h>

#include "contomap/model/Contomap.h"
#include "contomap/model/Filter.h"

#include "contomap/test/fixtures/ContomapViewFixture.h"
#include "contomap/test/samples/CoordinateSamples.h"

using contomap::model::Contomap;
using contomap::model::ContomapView;
using contomap::model::Filter;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Role;
using contomap::model::Topic;

using contomap::test::fixtures::ContomapViewFixture;
using contomap::test::samples::someSpacialCoordinate;

class ContomapTest : public testing::Test
{
public:
   ContomapTest()
      : map(Contomap::newMap())
      , viewFixture(map)
   {
   }

   ContomapViewFixture &view()
   {
      return viewFixture;
   }

protected:
   Contomap map;
   ContomapViewFixture viewFixture;
};

TEST_F(ContomapTest, emptyMapHasADefaultScope)
{
   auto topic = map.findTopic(map.getDefaultScope());
   EXPECT_TRUE(topic.has_value());
}

TEST_F(ContomapTest, topicsCanBeIterated)
{
   static_cast<void>(map.newTopic());
   static_cast<void>(map.newTopic());

   view().shouldHaveTopicCountOf(3);
}

TEST_F(ContomapTest, deletingAssociationsCleansUpRoles)
{
   auto &topic = map.newTopic();
   auto &association = map.newAssociation(Identifiers {}, someSpacialCoordinate());
   auto associationId = association.getId();
   static_cast<void>(topic.newRole(association));

   map.deleteAssociations(Identifiers::ofSingle(association.getId()));

   view().shouldHaveTopicThat(topic.getId(), [&associationId](Topic const &topic) {
      std::vector<Identifier> roles;
      std::ranges::copy(topic.rolesAssociatedWith(Identifiers::ofSingle(associationId)) | std::views::transform([](Role const &role) { return role.getId(); }),
         std::back_inserter(roles));
      EXPECT_THAT(roles, testing::SizeIs(0)) << "Roles are still assigned";
   });
}
