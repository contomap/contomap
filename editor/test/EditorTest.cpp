#include <gmock/gmock.h>

#include "contomap/editor/Editor.h"

#include "contomap/test/fixtures/ContomapViewFixture.h"
#include "contomap/test/matchers/Coordinates.h"
#include "contomap/test/matchers/Topics.h"
#include "contomap/test/printers/model.h"
#include "contomap/test/samples/CoordinateSamples.h"
#include "contomap/test/samples/TopicNameSamples.h"
#include "contomap/test/Steps.h"

#include "SelectionFixture.h"

using contomap::editor::Editor;
using contomap::editor::InputRequestHandler;
using contomap::editor::SelectedType;
using contomap::editor::Selection;
using contomap::editor::SelectionAction;
using contomap::editor::SelectionMode;
using contomap::model::Association;
using contomap::model::ContomapView;
using contomap::model::Filter;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::Role;
using contomap::model::SpacialCoordinate;
using contomap::model::Topic;
using contomap::model::TopicNameValue;

using contomap::test::fixtures::ContomapViewFixture;
using contomap::test::fixtures::SelectionFixture;
using contomap::test::matchers::hasName;
using contomap::test::matchers::isCloseTo;
using contomap::test::samples::someNameValue;
using contomap::test::samples::someSpacialCoordinate;

class EditorTest : public testing::Test, public contomap::test::Steps<EditorTest>
{
public:
   class NewTopicRequest
   {
   public:
      explicit NewTopicRequest(NewTopicRequest const &) = delete;
      explicit NewTopicRequest(NewTopicRequest &&) = delete;
      explicit NewTopicRequest(InputRequestHandler &handler)
         : handler(handler)
         , name(someNameValue())
         , position(someSpacialCoordinate())
      {
      }
      ~NewTopicRequest()
      {
         static_cast<void>(resolve());
      }

      NewTopicRequest &operator=(NewTopicRequest const &) = delete;
      NewTopicRequest &operator=(NewTopicRequest &&) = delete;

      operator Identifier()
      {
         return resolve();
      }

      NewTopicRequest &withName(std::string const &value)
      {
         name = std::get<TopicNameValue>(TopicNameValue::from(value));
         return *this;
      }

      NewTopicRequest &at(SpacialCoordinate value)
      {
         position = value;
         return *this;
      }

   private:
      [[nodiscard]] Identifier resolve()
      {
         if (!id.has_value())
         {
            id = handler.newTopicRequested(name, position);
         }
         return id.value();
      }

      InputRequestHandler &handler;
      std::optional<Identifier> id;
      TopicNameValue name;
      SpacialCoordinate position;
   };

   class NewAssociationRequest
   {
   public:
      explicit NewAssociationRequest(NewAssociationRequest const &) = delete;
      explicit NewAssociationRequest(NewAssociationRequest &&) = delete;
      explicit NewAssociationRequest(InputRequestHandler &handler)
         : handler(handler)
         , position(someSpacialCoordinate())
      {
      }
      ~NewAssociationRequest()
      {
         static_cast<void>(resolve());
      }

      NewAssociationRequest &operator=(NewAssociationRequest const &) = delete;
      NewAssociationRequest &operator=(NewAssociationRequest &&) = delete;

      operator Identifier()
      {
         return resolve();
      }

      NewAssociationRequest &at(SpacialCoordinate value)
      {
         position = value;
         return *this;
      }

   private:
      [[nodiscard]] Identifier resolve()
      {
         if (!id.has_value())
         {
            id = handler.newAssociationRequested(position);
         }
         return id.value();
      }

      InputRequestHandler &handler;
      std::optional<Identifier> id;
      SpacialCoordinate position;
   };

   class UserFixture
   {
   public:
      explicit UserFixture(contomap::editor::InputRequestHandler &handler)
         : handler(handler)
      {
      }

      NewTopicRequest requestsANewTopic()
      {
         return NewTopicRequest(handler);
      }

      NewAssociationRequest requestsANewAssociation()
      {
         return NewAssociationRequest(handler);
      }

      void linksTheSelection()
      {
         handler.linkSelection();
      }

      void deletesTheSelection()
      {
         handler.deleteSelection();
      }

      void selects(SelectedType type, Identifier id)
      {
         handler.modifySelection(type, id, SelectionAction::Set, SelectionMode::Sole);
      }

      void togglesSelectionOf(SelectedType type, Identifier id)
      {
         handler.modifySelection(type, id, SelectionAction::Toggle, SelectionMode::Sole);
      }

   private:
      InputRequestHandler &handler;
   };

   class ViewFixture
   {
   public:
      explicit ViewFixture(contomap::editor::View &view)
         : mapViewFixture(view.ofMap())
         , selectionFixture(view.ofSelection())
      {
      }

      ContomapViewFixture &ofMap()
      {
         return mapViewFixture;
      }

      SelectionFixture &ofSelection()
      {
         return selectionFixture;
      }

   private:
      ContomapViewFixture mapViewFixture;
      SelectionFixture selectionFixture;
   };

   EditorTest()
      : userFixture(instance)
      , viewFixture(instance)
   {
   }

   UserFixture &user()
   {
      return userFixture;
   }

   ViewFixture &view()
   {
      return viewFixture;
   }

   Identifiers viewScope()
   {
      return instance.ofViewScope();
   }

   Identifiers currentAssociations()
   {
      Identifiers ids;
      auto allAssociations = Filter<Association>::of([](Association const &, ContomapView const &) { return true; });
      std::ranges::for_each(instance.ofMap().find(std::move(allAssociations)), [&ids](Association const &association) { ids.add(association.getId()); });
      return ids;
   }

   Occurrence const &occurrenceOf(Identifier topicId)
   {
      std::reference_wrapper<Topic const> topic = instance.ofMap().findTopic(topicId).value();
      return *topic.get().occurrencesIn(viewScope()).begin();
   }

   Role const &roleOf(Identifier topicId, Identifier associationId)
   {
      std::vector<std::reference_wrapper<Role const>> roles;
      std::ranges::copy(instance.ofMap().findTopic(topicId).value().get().rolesAssociatedWith(Identifiers::ofSingle(associationId)), std::back_inserter(roles));
      return roles.at(0);
   }

private:
   Editor instance;
   UserFixture userFixture;
   ViewFixture viewFixture;
};

TEST_F(EditorTest, initialStateHasOneTopic)
{
   then().view().ofMap().shouldHaveTopicCountOf(1);
}

TEST_F(EditorTest, newTopicsCanBeCreated)
{
   when().user().requestsANewTopic();
   then().view().ofMap().shouldHaveTopicCountOf(2);
}

TEST_F(EditorTest, newTopicsKeepTheirProperties)
{
   auto position = someSpacialCoordinate();
   auto name = someNameValue();
   Identifier id = when().user().requestsANewTopic().withName(name.raw()).at(position);
   then().view().ofMap().shouldHaveTopicThat(id, [this, &id, &name, &position](Topic const &topic) {
      EXPECT_THAT(id, testing::Eq(topic.getId()));
      EXPECT_THAT(topic, hasName(name.raw()));
      std::vector<SpacialCoordinate> coordinates;
      std::ranges::copy(topic.occurrencesIn(viewScope()) | std::views::transform([](Occurrence const &o) { return o.getLocation().getSpacial(); }),
         std::back_inserter(coordinates));
      EXPECT_THAT(coordinates, testing::ElementsAre(isCloseTo(position)));
   });
}

TEST_F(EditorTest, newAssociationsKeepTheirProperties)
{
   auto position = someSpacialCoordinate();
   Identifier id = when().user().requestsANewAssociation().at(position);
   then().view().ofMap().shouldHaveAssociationThat(id, [&id, &position](Association const &association) {
      EXPECT_THAT(id, testing::Eq(association.getId()));
      EXPECT_THAT(association.getLocation().getSpacial(), isCloseTo(position));
   });
}

TEST_F(EditorTest, rolesCanBeCreatedBetweenTopicsAndAssociationsByLinkingThroughSelection)
{
   Identifier topicId = given().user().requestsANewTopic();
   Identifier associationId = given().user().requestsANewAssociation();
   given().user().selects(SelectedType::Occurrence, occurrenceOf(topicId).getId());
   given().user().togglesSelectionOf(SelectedType::Association, associationId);
   when().user().linksTheSelection();
   then().view().ofMap().shouldHaveTopicThat(topicId, [&associationId](Topic const &topic) {
      std::vector<Identifier> roles;
      std::ranges::copy(topic.rolesAssociatedWith(Identifiers::ofSingle(associationId)) | std::views::transform([](Role const &role) { return role.getId(); }),
         std::back_inserter(roles));
      EXPECT_THAT(roles, testing::SizeIs(1));
   });
}

TEST_F(EditorTest, newAssociationsIsCreatedBetweenSelectedTopicsByLinkingThroughSelection)
{
   Identifier topicId1 = given().user().requestsANewTopic();
   Identifier topicId2 = given().user().requestsANewTopic();
   given().user().selects(SelectedType::Occurrence, occurrenceOf(topicId1).getId());
   given().user().togglesSelectionOf(SelectedType::Occurrence, occurrenceOf(topicId2).getId());
   when().user().linksTheSelection();
   then().view().ofMap().shouldHaveTopicThat(topicId1, [this](Topic const &topic) {
      std::vector<Identifier> roles;
      std::ranges::copy(
         topic.rolesAssociatedWith(currentAssociations()) | std::views::transform([](Role const &role) { return role.getId(); }), std::back_inserter(roles));
      EXPECT_THAT(roles, testing::SizeIs(1));
   });
   asWellAs().view().ofMap().shouldHaveTopicThat(topicId2, [this](Topic const &topic) {
      std::vector<Identifier> roles;
      std::ranges::copy(
         topic.rolesAssociatedWith(currentAssociations()) | std::views::transform([](Role const &role) { return role.getId(); }), std::back_inserter(roles));
      EXPECT_THAT(roles, testing::SizeIs(1));
   });
}

TEST_F(EditorTest, newAssociationsCreatedByLinkingThroughSelectionIsCentredBetweenParticipants)
{
   Identifier topicId1 = given().user().requestsANewTopic().at(SpacialCoordinate::absoluteAt(-5.0f, 10.0f));
   Identifier topicId2 = given().user().requestsANewTopic().at(SpacialCoordinate::absoluteAt(0.0f, 20.0f));
   given().user().selects(SelectedType::Occurrence, occurrenceOf(topicId1).getId());
   given().user().togglesSelectionOf(SelectedType::Occurrence, occurrenceOf(topicId2).getId());
   when().user().linksTheSelection();
   then().view().ofMap().shouldHaveOneAssociationNear(SpacialCoordinate::absoluteAt(-2.5f, 15.0f));
}

TEST_F(EditorTest, newTopicHasItsOccurrenceSelected)
{
   when().user().requestsANewTopic();
   then().view().ofSelection().should([](Selection const &selection) {
      EXPECT_THAT(selection.of(SelectedType::Association), testing::Eq(Identifiers {}));
      EXPECT_THAT(selection.of(SelectedType::Role), testing::Eq(Identifiers {}));
      EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::Not(testing::Eq(Identifiers {})));
   });
}

TEST_F(EditorTest, newAssociationHasItSelected)
{
   when().user().requestsANewAssociation();
   then().view().ofSelection().should([](Selection const &selection) {
      EXPECT_THAT(selection.of(SelectedType::Association), testing::Not(testing::Eq(Identifiers {})));
      EXPECT_THAT(selection.of(SelectedType::Role), testing::Eq(Identifiers {}));
      EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::Eq(Identifiers {}));
   });
}

TEST_F(EditorTest, newAssociationThroughLinkingHasItSelected)
{
   Identifier topicId1 = given().user().requestsANewTopic().at(SpacialCoordinate::absoluteAt(-5.0f, 10.0f));
   Identifier topicId2 = given().user().requestsANewTopic().at(SpacialCoordinate::absoluteAt(0.0f, 20.0f));
   given().user().selects(SelectedType::Occurrence, occurrenceOf(topicId1).getId());
   given().user().togglesSelectionOf(SelectedType::Occurrence, occurrenceOf(topicId2).getId());
   when().user().linksTheSelection();
   then().view().ofSelection().should([](Selection const &selection) {
      EXPECT_THAT(selection.of(SelectedType::Association), testing::Not(testing::Eq(Identifiers {})));
      EXPECT_THAT(selection.of(SelectedType::Role), testing::Eq(Identifiers {}));
      EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::Eq(Identifiers {}));
   });
}

TEST_F(EditorTest, deletingAssociationRemovesTheAssociationAndRoles)
{
   Identifier topicId = given().user().requestsANewTopic();
   Identifier associationId = given().user().requestsANewAssociation();
   given().user().selects(SelectedType::Occurrence, occurrenceOf(topicId).getId());
   given().user().togglesSelectionOf(SelectedType::Association, associationId);
   given().user().linksTheSelection();
   given().user().selects(SelectedType::Association, associationId);
   when().user().deletesTheSelection();
   then().view().ofMap().shouldHaveAssociationCountOf(0);
   asWellAs().view().ofSelection().should(
      [](Selection const &selection) { EXPECT_THAT(selection.of(SelectedType::Association), testing::Eq(Identifiers {})) << "Association is still selected"; });
}

TEST_F(EditorTest, deletingLastOccurrenceRemovesTopic)
{
   Identifier topicId = given().user().requestsANewTopic();
   Identifier occurrenceId = occurrenceOf(topicId).getId();
   Identifier associationId = given().user().requestsANewAssociation();
   given().user().selects(SelectedType::Occurrence, occurrenceId);
   given().user().togglesSelectionOf(SelectedType::Association, associationId);
   given().user().linksTheSelection();
   given().user().selects(SelectedType::Occurrence, occurrenceId);
   when().user().deletesTheSelection();
   then().view().ofMap().shouldNotHaveTopic(topicId);
   asWellAs().view().ofSelection().should(
      [](Selection const &selection) { EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::Eq(Identifiers {})) << "Occurrence is still selected"; });
}

TEST_F(EditorTest, deletingRoleRemovesIt)
{
   Identifier topicId = given().user().requestsANewTopic();
   Identifier associationId = given().user().requestsANewAssociation();
   given().user().selects(SelectedType::Occurrence, occurrenceOf(topicId).getId());
   given().user().togglesSelectionOf(SelectedType::Association, associationId);
   given().user().linksTheSelection();

   given().user().selects(SelectedType::Role, roleOf(topicId, associationId).getId());
   when().user().deletesTheSelection();
   then().view().ofMap().shouldHaveTopicThat(topicId, [&associationId](Topic const &topic) {
      std::vector<Identifier> roles;
      std::ranges::copy(topic.rolesAssociatedWith(Identifiers::ofSingle(associationId)) | std::views::transform([](Role const &role) { return role.getId(); }),
         std::back_inserter(roles));
      EXPECT_THAT(roles, testing::SizeIs(0)) << "Role still assigned";
   });
   asWellAs().view().ofSelection().should(
      [](Selection const &selection) { EXPECT_THAT(selection.of(SelectedType::Role), testing::Eq(Identifiers {})) << "Role is still selected"; });
}