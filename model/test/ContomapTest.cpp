#include <gmock/gmock.h>

#include "contomap/model/Contomap.h"
#include "contomap/model/Filter.h"

#include "contomap/test/fixtures/ContomapViewFixture.h"
#include "contomap/test/samples/CoordinateSamples.h"

using contomap::model::Association;
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

   Identifiers defaultScope()
   {
      return Identifiers::ofSingle(map.getDefaultScopeTopic().getId());
   }

protected:
   Contomap map;
   ContomapViewFixture viewFixture;
};

TEST_F(ContomapTest, topicsCanBeIterated)
{
   static_cast<void>(map.newTopic());
   static_cast<void>(map.newTopic());

   view().shouldHaveTopicCountOf(3);
}

TEST_F(ContomapTest, deletingAssociationsCleansUpRoles)
{
   auto &topic = map.newTopic();
   auto &association = map.newAssociation(defaultScope(), someSpacialCoordinate());
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

TEST_F(ContomapTest, deletingOccurrenceWhileOthersRemain)
{
   auto &topic = map.newTopic();
   auto occurrenceId1 = topic.newOccurrence(defaultScope(), someSpacialCoordinate()).getId();
   auto occurrenceId2 = topic.newOccurrence(defaultScope(), someSpacialCoordinate()).getId();

   map.deleteOccurrences(Identifiers::ofSingle(occurrenceId1));

   view().shouldHaveTopicThat(topic.getId(), [occurrenceId1, occurrenceId2](Topic const &topic) {
      EXPECT_THAT(topic.occursAsAnyOf(Identifiers::ofSingle(occurrenceId1)), testing::Eq(false)) << "Topic should no longer occur through deleted instance";
      EXPECT_THAT(topic.occursAsAnyOf(Identifiers::ofSingle(occurrenceId2)), testing::Eq(true)) << "Topic should still occur through remaining instance";
   });
}

TEST_F(ContomapTest, deletingFinalOccurrenceRemovesTopic)
{
   auto &topic = map.newTopic();
   auto topicId = topic.getId();
   auto occurrenceId = topic.newOccurrence(defaultScope(), someSpacialCoordinate()).getId();

   map.deleteOccurrences(Identifiers::ofSingle(occurrenceId));

   view().shouldNotHaveTopic(topicId);
}

TEST_F(ContomapTest, deletingFinalOccurrenceOfDefaultViewScopeDoesNotDeleteIt)
{
   auto &topic = map.getDefaultScopeTopic();
   auto topicId = topic.getId();
   auto occurrenceId = topic.newOccurrence(defaultScope(), someSpacialCoordinate()).getId();

   map.deleteOccurrences(Identifiers::ofSingle(occurrenceId));

   view().shouldHaveTopicThat(topicId, [](Topic const &foundTopic) { EXPECT_TRUE(foundTopic.isWithoutOccurrences()); });
}

TEST_F(ContomapTest, removingAScopeTopicCascadesIntoFurtherRemovalOfOtherTopics)
{
   auto &topicA = map.newTopic();
   auto &topicB = map.newTopic();
   auto topicIdA = topicA.getId();
   auto topicIdB = topicB.getId();
   static_cast<void>(topicA.newOccurrence(Identifiers::ofSingle(topicIdB), someSpacialCoordinate()).getId());
   auto occurrenceId = topicB.newOccurrence(defaultScope(), someSpacialCoordinate()).getId();

   map.deleteOccurrences(Identifiers::ofSingle(occurrenceId));

   view().shouldNotHaveTopic(topicIdA);
   view().shouldNotHaveTopic(topicIdB);
}

TEST_F(ContomapTest, removingAScopeTopicCascadesIntoFurtherRemovalOfAssociations)
{
   auto &topicA = map.newTopic();
   auto &topicB = map.newTopic();
   auto topicIdB = topicB.getId();
   static_cast<void>(topicA.newOccurrence(Identifiers::ofSingle(topicIdB), someSpacialCoordinate()).getId());
   auto &association = map.newAssociation(Identifiers::ofSingle(topicIdB), someSpacialCoordinate());
   auto associationId = association.getId();
   static_cast<void>(topicA.newRole(association));
   auto occurrenceId = topicB.newOccurrence(defaultScope(), someSpacialCoordinate()).getId();

   map.deleteOccurrences(Identifiers::ofSingle(occurrenceId));

   view().shouldNotHaveAssociation(associationId);
}

TEST_F(ContomapTest, removingACyclicDependencyWorks)
{
   auto &topicA = map.newTopic();
   auto &topicB = map.newTopic();
   auto &topicC = map.newTopic();
   auto topicIdA = topicA.getId();
   auto topicIdB = topicB.getId();
   auto topicIdC = topicC.getId();
   static_cast<void>(topicA.newOccurrence(Identifiers::ofSingle(topicIdB), someSpacialCoordinate()).getId());
   static_cast<void>(topicB.newOccurrence(Identifiers::ofSingle(topicIdC), someSpacialCoordinate()).getId());
   auto occurrenceId = topicC.newOccurrence(Identifiers::ofSingle(topicIdA), someSpacialCoordinate()).getId();

   map.deleteOccurrences(Identifiers::ofSingle(occurrenceId));

   view().shouldNotHaveTopic(topicIdA);
   view().shouldNotHaveTopic(topicIdB);
   view().shouldNotHaveTopic(topicIdC);
}

TEST_F(ContomapTest, rolesOfRemovedTopicAreAlsoRemoved)
{
   auto &topic = map.newTopic();
   auto occurrenceId = topic.newOccurrence(defaultScope(), someSpacialCoordinate()).getId();
   auto &association = map.newAssociation(defaultScope(), someSpacialCoordinate());
   static_cast<void>(topic.newRole(association));

   map.deleteOccurrences(Identifiers::ofSingle(occurrenceId));

   view().shouldHaveAssociationThat(
      association.getId(), [](Association const &association) { EXPECT_FALSE(association.hasRoles()) << "Association should not have any roles left"; });
}

TEST_F(ContomapTest, rolesCanBeRemoved)
{
   auto &topic = map.newTopic();
   static_cast<void>(topic.newOccurrence(defaultScope(), someSpacialCoordinate()).getId());
   auto &association = map.newAssociation(defaultScope(), someSpacialCoordinate());
   auto roleId1 = topic.newRole(association).getId();
   auto roleId2 = topic.newRole(association).getId();

   map.deleteRoles(Identifiers::ofSingle(roleId1));

   view().shouldHaveTopicThat(topic.getId(), [&association, roleId2](Topic const &topic) {
      auto rolesView = std::ranges::common_view(topic.rolesAssociatedWith(Identifiers::ofSingle(association.getId())));
      std::vector<std::reference_wrapper<Role const>> roles(rolesView.begin(), rolesView.end());
      ASSERT_THAT(roles, testing::SizeIs(1));
      EXPECT_THAT(roles[0].get().getId(), testing::Eq(roleId2));
   });

   EXPECT_TRUE(association.hasRoles()) << "Association should still have roles";
}
