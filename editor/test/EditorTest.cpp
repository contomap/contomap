#include <gmock/gmock.h>

#include "contomap/editor/Editor.h"
#include "contomap/infrastructure/serial/BinaryDecoder.h"
#include "contomap/infrastructure/serial/BinaryEncoder.h"

#include "contomap/test/fixtures/ContomapViewFixture.h"
#include "contomap/test/matchers/Coordinates.h"
#include "contomap/test/matchers/Topics.h"
#include "contomap/test/printers/model.h"
#include "contomap/test/samples/CoordinateSamples.h"
#include "contomap/test/samples/TopicNameSamples.h"
#include "contomap/test/Steps.h"

#include "SelectionFixture.h"
#include "ViewScopeFixture.h"

using contomap::editor::Editor;
using contomap::editor::InputRequestHandler;
using contomap::editor::SelectedType;
using contomap::editor::Selection;
using contomap::editor::SelectionAction;
using contomap::infrastructure::serial::BinaryDecoder;
using contomap::infrastructure::serial::BinaryEncoder;
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
using contomap::test::fixtures::ViewScopeFixture;
using contomap::test::matchers::hasDefaultName;
using contomap::test::matchers::hasName;
using contomap::test::matchers::hasNameCountOf;
using contomap::test::matchers::hasScopedName;
using contomap::test::matchers::isCloseTo;
using contomap::test::samples::someNameValue;
using contomap::test::samples::someSpacialCoordinate;

class EditorTest : public testing::TestWithParam<bool>, public contomap::test::Steps<EditorTest>
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

   class NewOccurrenceRequest
   {
   public:
      explicit NewOccurrenceRequest(NewOccurrenceRequest const &) = delete;
      explicit NewOccurrenceRequest(NewOccurrenceRequest &&) = delete;
      explicit NewOccurrenceRequest(InputRequestHandler &handler, Identifier topicId)
         : handler(handler)
         , topicId(topicId)
         , position(someSpacialCoordinate())
      {
      }
      ~NewOccurrenceRequest()
      {
         resolve();
      }

      NewOccurrenceRequest &operator=(NewOccurrenceRequest const &) = delete;
      NewOccurrenceRequest &operator=(NewOccurrenceRequest &&) = delete;

      NewOccurrenceRequest &at(SpacialCoordinate value)
      {
         position = value;
         return *this;
      }

   private:
      void resolve()
      {
         if (!resolved)
         {
            handler.newOccurrenceRequested(topicId, position);
            resolved = true;
         }
      }

      InputRequestHandler &handler;
      Identifier topicId;
      bool resolved = false;
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
      explicit UserFixture(InputRequestHandler &handler)
         : handler(handler)
      {
      }

      NewTopicRequest requestsANewTopic()
      {
         return NewTopicRequest(handler);
      }

      void setsTopicNameDefaultTo(Identifier topicId, TopicNameValue const &value)
      {
         handler.setTopicNameDefault(topicId, value);
      }

      void setsTopicNameInScopeTo(Identifier topicId, TopicNameValue const &value)
      {
         handler.setTopicNameInScope(topicId, value);
      }

      void removesTopicNameInScopeOf(Identifier topicId)
      {
         handler.removeTopicNameInScope(topicId);
      }

      NewOccurrenceRequest requestsANewOccurrence(Identifier id)
      {
         return NewOccurrenceRequest(handler, id);
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

      void setsTypeOfSelectionTo(Identifier id)
      {
         handler.setTypeOfSelection(id);
      }

      void clearsTypeOfSelection()
      {
         handler.clearTypeOfSelection();
      }

      void setsTheViewScopeFromSelection()
      {
         handler.setViewScopeFromSelection();
      }

      void setsTheViewScopeToDefault()
      {
         handler.setViewScopeToDefault();
      }

      void selects(SelectedType type, Identifier id)
      {
         handler.modifySelection(type, id, SelectionAction::Set);
      }

      void togglesSelectionOf(SelectedType type, Identifier id)
      {
         handler.modifySelection(type, id, SelectionAction::Toggle);
      }

      void movesTheSelectionBy(SpacialCoordinate::Offset offset)
      {
         handler.moveSelectionBy(offset);
      }

      void setsTheViewScopeToBe(Identifier id)
      {
         handler.setViewScopeTo(id);
      }

      void addsToTheViewScope(Identifier id)
      {
         handler.addToViewScope(id);
      }

      void removesFromTheViewScope(Identifier id)
      {
         handler.removeFromViewScope(id);
      }

      void cyclesTheSelectedOccurrenceForward()
      {
         handler.cycleSelectedOccurrenceForward();
      }

      void cyclesTheSelectedOccurrenceReverse()
      {
         handler.cycleSelectedOccurrenceReverse();
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
         , viewScopeFixture(view.ofViewScope())
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

      ViewScopeFixture &ofViewScope()
      {
         return viewScopeFixture;
      }

   private:
      ContomapViewFixture mapViewFixture;
      SelectionFixture selectionFixture;
      ViewScopeFixture viewScopeFixture;
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
      saveAndLoadState();
      return viewFixture;
   }

   Identifier defaultViewScope()
   {
      return instance.ofMap().getDefaultScope();
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
   void saveAndLoadState()
   {
      bool withSerialization = GetParam();
      if (withSerialization)
      {
         BinaryEncoder encoder;
         instance.saveState(encoder, true);
         auto const &data = encoder.getData();
         BinaryDecoder decoder(data.data(), data.data() + data.size());
         bool result = instance.loadState(decoder);
         ASSERT_TRUE(result) << "could not properly restore state by itself";
      }
   }

   Editor instance;
   UserFixture userFixture;
   ViewFixture viewFixture;
};

INSTANTIATE_TEST_SUITE_P(ParameterizedEditorTest, EditorTest, testing::Values(false, true),
   [](testing::TestParamInfo<EditorTest::ParamType> const &info) { return std::string("WithSerialization_") + (info.param ? "Yes" : "No"); });

TEST_P(EditorTest, initialStateHasOneTopic)
{
   then().view().ofMap().shouldHaveTopicCountOf(1);
}

TEST_P(EditorTest, newTopicsCanBeCreated)
{
   when().user().requestsANewTopic();
   then().view().ofMap().shouldHaveTopicCountOf(2);
}

TEST_P(EditorTest, newTopicsKeepTheirProperties)
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

TEST_P(EditorTest, topicsCanReceiveNewOccurrences)
{
   Identifier id = given().user().requestsANewTopic();
   when().user().requestsANewOccurrence(id);
   then().view().ofMap().shouldHaveTopicThat(id, [this](Topic const &topic) {
      auto occurrencesView = std::ranges::common_view(topic.occurrencesIn(viewScope()));
      std::vector<std::reference_wrapper<Occurrence const>> occurrences(occurrencesView.begin(), occurrencesView.end());
      EXPECT_THAT(occurrences, testing::SizeIs(2));
   });
}

TEST_P(EditorTest, newAssociationsKeepTheirProperties)
{
   auto position = someSpacialCoordinate();
   Identifier id = when().user().requestsANewAssociation().at(position);
   then().view().ofMap().shouldHaveAssociationThat(id, [&id, &position](Association const &association) {
      EXPECT_THAT(id, testing::Eq(association.getId()));
      EXPECT_THAT(association.getLocation().getSpacial(), isCloseTo(position));
   });
}

TEST_P(EditorTest, rolesCanBeCreatedBetweenTopicsAndAssociationsByLinkingThroughSelection)
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

TEST_P(EditorTest, newAssociationsIsCreatedBetweenSelectedTopicsByLinkingThroughSelection)
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

TEST_P(EditorTest, newAssociationsCreatedByLinkingThroughSelectionIsCentredBetweenParticipants)
{
   Identifier topicId1 = given().user().requestsANewTopic().at(SpacialCoordinate::absoluteAt(-5.0f, 10.0f));
   Identifier topicId2 = given().user().requestsANewTopic().at(SpacialCoordinate::absoluteAt(0.0f, 20.0f));
   given().user().selects(SelectedType::Occurrence, occurrenceOf(topicId1).getId());
   given().user().togglesSelectionOf(SelectedType::Occurrence, occurrenceOf(topicId2).getId());
   when().user().linksTheSelection();
   then().view().ofMap().shouldHaveOneAssociationNear(SpacialCoordinate::absoluteAt(-2.5f, 15.0f));
}

TEST_P(EditorTest, newAssociationsCreatedByLinkingThroughSelectionIsCentredBetweenAllSelectedOccurrences)
{
   Identifier topicId1 = given().user().requestsANewTopic().at(SpacialCoordinate::absoluteAt(-5.0f, 10.0f));
   Identifier topicId2 = given().user().requestsANewTopic().at(SpacialCoordinate::absoluteAt(0.0f, 20.0f));
   auto const &firstTopic1Occurrence = occurrenceOf(topicId1);
   given().user().requestsANewOccurrence(topicId1).at(SpacialCoordinate::absoluteAt(-10.0f, 3.0f));
   given().user().selects(SelectedType::Occurrence, firstTopic1Occurrence.getId());
   given().user().togglesSelectionOf(SelectedType::Occurrence, occurrenceOf(topicId2).getId());
   when().user().linksTheSelection();
   then().view().ofMap().shouldHaveOneAssociationNear(SpacialCoordinate::absoluteAt(-2.5f, 15.0f));
}

TEST_P(EditorTest, newTopicHasItsOccurrenceSelected)
{
   when().user().requestsANewTopic();
   then().view().ofSelection().should([](Selection const &selection) {
      EXPECT_THAT(selection.of(SelectedType::Association), testing::Eq(Identifiers {}));
      EXPECT_THAT(selection.of(SelectedType::Role), testing::Eq(Identifiers {}));
      EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::Not(testing::Eq(Identifiers {})));
   });
}

TEST_P(EditorTest, newAssociationHasItSelected)
{
   when().user().requestsANewAssociation();
   then().view().ofSelection().should([](Selection const &selection) {
      EXPECT_THAT(selection.of(SelectedType::Association), testing::Not(testing::Eq(Identifiers {})));
      EXPECT_THAT(selection.of(SelectedType::Role), testing::Eq(Identifiers {}));
      EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::Eq(Identifiers {}));
   });
}

TEST_P(EditorTest, newAssociationThroughLinkingHasItSelected)
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

TEST_P(EditorTest, deletingAssociationRemovesTheAssociationAndRoles)
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

TEST_P(EditorTest, deletingLastOccurrenceRemovesTopic)
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

TEST_P(EditorTest, deletingRoleRemovesIt)
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

TEST_P(EditorTest, setViewScopeFromSelectionClearsSelection)
{
   Identifier topicId = given().user().requestsANewTopic();
   given().user().selects(SelectedType::Occurrence, occurrenceOf(topicId).getId());
   when().user().setsTheViewScopeFromSelection();
   then().view().ofSelection().should(
      [](Selection const &selection) { EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::Eq(Identifiers {})) << "Occurrence is still selected"; });
}

TEST_P(EditorTest, newOccurrencesAreInNewViewScope)
{
   Identifier scopeTopicId = given().user().requestsANewTopic();
   given().user().selects(SelectedType::Occurrence, occurrenceOf(scopeTopicId).getId());
   given().user().setsTheViewScopeFromSelection();
   Identifier topicId = when().user().requestsANewTopic();
   then().view().ofMap().shouldHaveTopicThat(
      topicId, [this](Topic const &topic) { EXPECT_TRUE(topic.isIn(viewScope())) << "Topic has no occurrence in this new scope"; });
}

TEST_P(EditorTest, setViewScopeToSingleTopic)
{
   Identifier scopeTopicId = given().user().requestsANewTopic();
   when().user().setsTheViewScopeToBe(scopeTopicId);
   then().view().ofViewScope().shouldBe(Identifiers::ofSingle(scopeTopicId));
}

TEST_P(EditorTest, unknownTopicsCanNotBeSetToBeTheViewScope)
{
   Identifiers originalScope = viewScope();
   Identifier unknownId = Identifier::random();
   when().user().setsTheViewScopeToBe(unknownId);
   then().view().ofViewScope().shouldBe(originalScope);
}

TEST_P(EditorTest, addViewScopeOfSingleTopic)
{
   Identifier scopeTopicId = given().user().requestsANewTopic();
   when().user().addsToTheViewScope(scopeTopicId);
   then().view().ofViewScope().shouldContain(scopeTopicId);
   asWellAs().view().ofViewScope().shouldContain(defaultViewScope());
}

TEST_P(EditorTest, unknownTopicsCanNotBeAddedToTheViewScope)
{
   Identifiers originalScope = viewScope();
   Identifier unknownId = Identifier::random();
   when().user().addsToTheViewScope(unknownId);
   then().view().ofViewScope().shouldBe(originalScope);
}

TEST_P(EditorTest, removeSingleTopicFromViewScope)
{
   Identifier scopeTopicId = given().user().requestsANewTopic();
   given().user().addsToTheViewScope(scopeTopicId);
   when().user().removesFromTheViewScope(defaultViewScope());
   then().view().ofViewScope().shouldBe(Identifiers::ofSingle(scopeTopicId));
}

TEST_P(EditorTest, removingLastTopicFromViewScopeResetsToDefault)
{
   Identifier scopeTopicId = given().user().requestsANewTopic();
   given().user().addsToTheViewScope(scopeTopicId);
   given().user().removesFromTheViewScope(defaultViewScope());
   when().user().removesFromTheViewScope(scopeTopicId);
   then().view().ofViewScope().shouldBe(Identifiers::ofSingle(defaultViewScope()));
}

TEST_P(EditorTest, deletingTopicOfCurrentViewScopeRemovesItFromViewScope)
{
   Identifier scopeTopicId = given().user().requestsANewTopic();
   Identifier firstOccurrenceIdOfScopeTopic = occurrenceOf(scopeTopicId).getId();
   given().user().setsTheViewScopeToBe(scopeTopicId);
   given().user().requestsANewOccurrence(scopeTopicId);
   given().user().setsTheViewScopeToDefault();
   given().user().selects(SelectedType::Occurrence, firstOccurrenceIdOfScopeTopic);
   given().user().deletesTheSelection();
   given().user().setsTheViewScopeToBe(scopeTopicId);
   given().user().selects(SelectedType::Occurrence, occurrenceOf(scopeTopicId).getId());
   when().user().deletesTheSelection();
   then().view().ofViewScope().shouldBe(Identifiers::ofSingle(defaultViewScope()));
}

TEST_P(EditorTest, cyclingThroughOccurrencesSingleForward)
{
   Identifier topicId = given().user().requestsANewTopic();
   Identifier occurrenceId = occurrenceOf(topicId).getId();
   given().user().selects(SelectedType::Occurrence, occurrenceId);
   when().user().cyclesTheSelectedOccurrenceForward();
   then().view().ofSelection().should(
      [occurrenceId](Selection const &selection) { EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::Eq(Identifiers::ofSingle(occurrenceId))); });
}

TEST_P(EditorTest, cyclingThroughOccurrencesSingleReverse)
{
   Identifier topicId = given().user().requestsANewTopic();
   Identifier occurrenceId = occurrenceOf(topicId).getId();
   given().user().selects(SelectedType::Occurrence, occurrenceId);
   when().user().cyclesTheSelectedOccurrenceReverse();
   then().view().ofSelection().should(
      [occurrenceId](Selection const &selection) { EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::Eq(Identifiers::ofSingle(occurrenceId))); });
}

TEST_P(EditorTest, cyclingThroughOccurrencesToSecondInSameScopeForward)
{
   Identifier topicId = given().user().requestsANewTopic();
   Identifier occurrenceId = occurrenceOf(topicId).getId();
   given().user().requestsANewOccurrence(topicId);
   given().user().selects(SelectedType::Occurrence, occurrenceId);
   when().user().cyclesTheSelectedOccurrenceForward();
   then().view().ofSelection().should([occurrenceId](Selection const &selection) {
      EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::SizeIs(1));
      EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::Not(testing::Eq(Identifiers::ofSingle(occurrenceId))));
   });
}

TEST_P(EditorTest, cyclingThroughOccurrencesToSecondInSameScopeReverse)
{
   Identifier topicId = given().user().requestsANewTopic();
   Identifier occurrenceId = occurrenceOf(topicId).getId();
   given().user().requestsANewOccurrence(topicId);
   given().user().requestsANewOccurrence(topicId);
   given().user().selects(SelectedType::Occurrence, occurrenceId);
   given().user().cyclesTheSelectedOccurrenceForward();
   given().user().cyclesTheSelectedOccurrenceReverse();
   then().view().ofSelection().should([occurrenceId](Selection const &selection) {
      EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::SizeIs(1));
      EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::Eq(Identifiers::ofSingle(occurrenceId)));
   });
}

TEST_P(EditorTest, cyclingThroughOccurrencesBackToOriginalInSameScope)
{
   Identifier topicId = given().user().requestsANewTopic();
   Identifier occurrenceId = occurrenceOf(topicId).getId();
   given().user().requestsANewOccurrence(topicId);
   given().user().selects(SelectedType::Occurrence, occurrenceId);
   given().user().cyclesTheSelectedOccurrenceForward();
   when().user().cyclesTheSelectedOccurrenceForward();
   then().view().ofSelection().should(
      [occurrenceId](Selection const &selection) { EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::Eq(Identifiers::ofSingle(occurrenceId))); });
}

TEST_P(EditorTest, cyclingThroughOccurrencesIsIgnoredIfMoreThanOneOccurrenceSelected)
{
   Identifier topicId1 = given().user().requestsANewTopic();
   Identifier occurrenceId1 = occurrenceOf(topicId1).getId();
   given().user().requestsANewOccurrence(topicId1);
   Identifier topicId2 = given().user().requestsANewTopic();
   Identifier occurrenceId2 = occurrenceOf(topicId2).getId();
   given().user().selects(SelectedType::Occurrence, occurrenceId1);
   given().user().togglesSelectionOf(SelectedType::Occurrence, occurrenceId2);
   when().user().cyclesTheSelectedOccurrenceForward();
   then().view().ofSelection().should([](Selection const &selection) { EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::SizeIs(2)); });
}

TEST_P(EditorTest, cyclingThroughOccurrencesToSecondInOtherScope)
{
   Identifier scopeTopicId = given().user().requestsANewTopic();
   Identifier topicId = given().user().requestsANewTopic();
   Identifier occurrenceId = occurrenceOf(topicId).getId();
   given().user().setsTheViewScopeToBe(scopeTopicId);
   given().user().requestsANewOccurrence(topicId);
   given().user().setsTheViewScopeToDefault();
   given().user().selects(SelectedType::Occurrence, occurrenceId);
   when().user().cyclesTheSelectedOccurrenceForward();
   then().view().ofSelection().should([occurrenceId](Selection const &selection) {
      EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::SizeIs(1));
      EXPECT_THAT(selection.of(SelectedType::Occurrence), testing::Not(testing::Eq(Identifiers::ofSingle(occurrenceId))));
   });
   asWellAs().view().ofViewScope().shouldBe(Identifiers::ofSingle(scopeTopicId));
}

TEST_P(EditorTest, defaultTopicNameCanBeChanged)
{
   Identifier topicId = given().user().requestsANewTopic();
   auto newName = someNameValue();
   when().user().setsTopicNameDefaultTo(topicId, newName);
   then().view().ofMap().shouldHaveTopicThat(topicId, [newName](Topic const &topic) {
      EXPECT_THAT(topic, hasNameCountOf(1));
      EXPECT_THAT(topic, hasDefaultName(newName.raw()));
   });
}

TEST_P(EditorTest, scopedTopicNameCanBeSet)
{
   Identifier scopeTopicId = given().user().requestsANewTopic();
   Identifier topicId = given().user().requestsANewTopic();
   given().user().setsTheViewScopeToBe(scopeTopicId);
   auto newName = someNameValue();
   when().user().setsTopicNameInScopeTo(topicId, newName);
   then().view().ofMap().shouldHaveTopicThat(topicId, [scopeTopicId, newName](Topic const &topic) {
      EXPECT_THAT(topic, hasNameCountOf(2));
      EXPECT_THAT(topic, hasScopedName(Identifiers::ofSingle(scopeTopicId), newName.raw()));
   });
}

TEST_P(EditorTest, scopedTopicNameCanBeChanged)
{
   Identifier scopeTopicId = given().user().requestsANewTopic();
   Identifier topicId = given().user().requestsANewTopic();
   given().user().setsTheViewScopeToBe(scopeTopicId);
   given().user().setsTopicNameInScopeTo(topicId, someNameValue());
   auto newName = someNameValue();
   when().user().setsTopicNameInScopeTo(topicId, newName);
   then().view().ofMap().shouldHaveTopicThat(topicId, [scopeTopicId, newName](Topic const &topic) {
      EXPECT_THAT(topic, hasNameCountOf(2));
      EXPECT_THAT(topic, hasScopedName(Identifiers::ofSingle(scopeTopicId), newName.raw()));
   });
}

TEST_P(EditorTest, scopedTopicNameCanBeRemoved)
{
   Identifier scopeTopicId = given().user().requestsANewTopic();
   std::string defaultName("defaultName");
   Identifier topicId = given().user().requestsANewTopic().withName(defaultName);
   given().user().setsTheViewScopeToBe(scopeTopicId);
   given().user().setsTopicNameInScopeTo(topicId, someNameValue());
   when().user().removesTopicNameInScopeOf(topicId);
   then().view().ofMap().shouldHaveTopicThat(topicId, [defaultName](Topic const &topic) {
      EXPECT_THAT(topic, hasNameCountOf(1));
      EXPECT_THAT(topic, hasDefaultName(defaultName));
   });
}

TEST_P(EditorTest, scopedTopicNameIsRemovedIfScopeIsDeleted)
{
   Identifier scopeTopicId = given().user().requestsANewTopic();
   std::string defaultName("defaultName");
   Identifier topicId = given().user().requestsANewTopic().withName(defaultName);
   given().user().setsTheViewScopeToBe(scopeTopicId);
   given().user().setsTopicNameInScopeTo(topicId, someNameValue());
   given().user().setsTheViewScopeToDefault();
   given().user().selects(SelectedType::Occurrence, occurrenceOf(scopeTopicId).getId());
   given().user().deletesTheSelection();
   then().view().ofMap().shouldHaveTopicThat(topicId, [defaultName](Topic const &topic) {
      EXPECT_THAT(topic, hasNameCountOf(1));
      EXPECT_THAT(topic, hasDefaultName(defaultName));
   });
}

TEST_P(EditorTest, settingTypeOfOccurrence)
{
   Identifier typeTopicId = given().user().requestsANewTopic();
   Identifier topicId = given().user().requestsANewTopic();
   Identifier occurrenceId = occurrenceOf(topicId).getId();
   given().user().selects(SelectedType::Occurrence, occurrenceId);
   when().user().setsTypeOfSelectionTo(typeTopicId);
   then().view().ofMap().shouldHaveTopicThat(topicId, [occurrenceId, typeTopicId](Topic const &topic) {
      auto optionalType = topic.getOccurrence(occurrenceId)->get().getType();
      ASSERT_TRUE(optionalType.has_value());
      EXPECT_EQ(optionalType.value().get().getId(), typeTopicId);
   });
}

TEST_P(EditorTest, clearingTypeOfOccurrence)
{
   Identifier typeTopicId = given().user().requestsANewTopic();
   Identifier topicId = given().user().requestsANewTopic();
   Identifier occurrenceId = occurrenceOf(topicId).getId();
   given().user().selects(SelectedType::Occurrence, occurrenceId);
   given().user().setsTypeOfSelectionTo(typeTopicId);
   when().user().clearsTypeOfSelection();
   then().view().ofMap().shouldHaveTopicThat(topicId, [occurrenceId](Topic const &topic) {
      auto optionalType = topic.getOccurrence(occurrenceId)->get().getType();
      ASSERT_FALSE(optionalType.has_value());
   });
}

TEST_P(EditorTest, deletingTypeOfOccurrenceClearsIt)
{
   Identifier typeTopicId = given().user().requestsANewTopic();
   Identifier topicId = given().user().requestsANewTopic();
   Identifier occurrenceId = occurrenceOf(topicId).getId();
   given().user().selects(SelectedType::Occurrence, occurrenceId);
   given().user().setsTypeOfSelectionTo(typeTopicId);
   given().user().selects(SelectedType::Occurrence, occurrenceOf(typeTopicId).getId());
   when().user().deletesTheSelection();
   then().view().ofMap().shouldHaveTopicThat(topicId, [occurrenceId](Topic const &topic) {
      auto optionalType = topic.getOccurrence(occurrenceId)->get().getType();
      ASSERT_FALSE(optionalType.has_value());
   });
}

TEST_P(EditorTest, typeOfOccurrenceCanNotBeSetToUnknownId)
{
   Identifier typeTopicId = Identifier::random();
   Identifier topicId = given().user().requestsANewTopic();
   Identifier occurrenceId = occurrenceOf(topicId).getId();
   given().user().selects(SelectedType::Occurrence, occurrenceId);
   when().user().setsTypeOfSelectionTo(typeTopicId);
   then().view().ofMap().shouldHaveTopicThat(topicId, [occurrenceId](Topic const &topic) {
      auto optionalType = topic.getOccurrence(occurrenceId)->get().getType();
      ASSERT_FALSE(optionalType.has_value());
   });
}

TEST_P(EditorTest, settingTypeOfAssociation)
{
   Identifier typeTopicId = given().user().requestsANewTopic();
   Identifier associationId = given().user().requestsANewAssociation();
   given().user().selects(SelectedType::Association, associationId);
   when().user().setsTypeOfSelectionTo(typeTopicId);
   then().view().ofMap().shouldHaveAssociationThat(associationId, [typeTopicId](Association const &association) {
      auto optionalType = association.getType();
      ASSERT_TRUE(optionalType.isAssigned());
      EXPECT_EQ(optionalType.value(), typeTopicId);
   });
}

TEST_P(EditorTest, clearingTypeOfAssociation)
{
   Identifier typeTopicId = given().user().requestsANewTopic();
   Identifier associationId = given().user().requestsANewAssociation();
   given().user().selects(SelectedType::Association, associationId);
   given().user().setsTypeOfSelectionTo(typeTopicId);
   when().user().clearsTypeOfSelection();
   then().view().ofMap().shouldHaveAssociationThat(associationId, [](Association const &association) {
      auto optionalType = association.getType();
      ASSERT_FALSE(optionalType.isAssigned());
   });
}

TEST_P(EditorTest, deletingTypeOfAssociationClearsIt)
{
   Identifier typeTopicId = given().user().requestsANewTopic();
   Identifier associationId = given().user().requestsANewAssociation();
   given().user().selects(SelectedType::Association, associationId);
   given().user().setsTypeOfSelectionTo(typeTopicId);
   given().user().selects(SelectedType::Occurrence, occurrenceOf(typeTopicId).getId());
   when().user().deletesTheSelection();
   then().view().ofMap().shouldHaveAssociationThat(associationId, [](Association const &association) {
      auto optionalType = association.getType();
      ASSERT_FALSE(optionalType.isAssigned());
   });
}

TEST_P(EditorTest, typeOfAssociationCanNotBeSetToUnknownId)
{
   Identifier typeTopicId = Identifier::random();
   Identifier associationId = given().user().requestsANewAssociation();
   given().user().selects(SelectedType::Association, associationId);
   when().user().setsTypeOfSelectionTo(typeTopicId);
   then().view().ofMap().shouldHaveAssociationThat(associationId, [](Association const &association) {
      auto optionalType = association.getType();
      ASSERT_FALSE(optionalType.isAssigned());
   });
}

TEST_P(EditorTest, settingTypeOfRole)
{
   Identifier typeTopicId = given().user().requestsANewTopic();
   Identifier topicId = given().user().requestsANewTopic();
   Identifier associationId = given().user().requestsANewAssociation();
   given().user().selects(SelectedType::Occurrence, occurrenceOf(topicId).getId());
   given().user().togglesSelectionOf(SelectedType::Association, associationId);
   given().user().linksTheSelection();
   Identifier roleId = roleOf(topicId, associationId).getId();
   given().user().selects(SelectedType::Role, roleId);
   when().user().setsTypeOfSelectionTo(typeTopicId);
   then().view().ofMap().shouldHaveTopicThat(topicId, [roleId, typeTopicId](Topic const &topic) {
      Role const &role = *topic.findRoles(Identifiers::ofSingle(roleId)).begin();
      auto optionalType = role.getType();
      ASSERT_TRUE(optionalType.has_value());
      EXPECT_EQ(optionalType.value().get().getId(), typeTopicId);
   });
}

TEST_P(EditorTest, clearingTypeOfRole)
{
   Identifier typeTopicId = given().user().requestsANewTopic();
   Identifier topicId = given().user().requestsANewTopic();
   Identifier associationId = given().user().requestsANewAssociation();
   given().user().selects(SelectedType::Occurrence, occurrenceOf(topicId).getId());
   given().user().togglesSelectionOf(SelectedType::Association, associationId);
   given().user().linksTheSelection();
   Identifier roleId = roleOf(topicId, associationId).getId();
   given().user().selects(SelectedType::Role, roleId);
   given().user().setsTypeOfSelectionTo(typeTopicId);
   when().user().clearsTypeOfSelection();
   then().view().ofMap().shouldHaveTopicThat(topicId, [roleId](Topic const &topic) {
      Role const &role = *topic.findRoles(Identifiers::ofSingle(roleId)).begin();
      auto optionalType = role.getType();
      ASSERT_FALSE(optionalType.has_value());
   });
}

TEST_P(EditorTest, deletingTypeOfRoleClearsIt)
{
   Identifier typeTopicId = given().user().requestsANewTopic();
   Identifier topicId = given().user().requestsANewTopic();
   Identifier associationId = given().user().requestsANewAssociation();
   given().user().selects(SelectedType::Occurrence, occurrenceOf(topicId).getId());
   given().user().togglesSelectionOf(SelectedType::Association, associationId);
   given().user().linksTheSelection();
   Identifier roleId = roleOf(topicId, associationId).getId();
   given().user().selects(SelectedType::Role, roleId);
   given().user().setsTypeOfSelectionTo(typeTopicId);
   given().user().selects(SelectedType::Occurrence, occurrenceOf(typeTopicId).getId());
   when().user().deletesTheSelection();
   then().view().ofMap().shouldHaveTopicThat(topicId, [roleId](Topic const &topic) {
      Role const &role = *topic.findRoles(Identifiers::ofSingle(roleId)).begin();
      auto optionalType = role.getType();
      ASSERT_FALSE(optionalType.has_value());
   });
}

TEST_P(EditorTest, typeOfRoleCanNotBeSetToUnknownId)
{
   Identifier typeTopicId = Identifier::random();
   Identifier topicId = given().user().requestsANewTopic();
   Identifier associationId = given().user().requestsANewAssociation();
   given().user().selects(SelectedType::Occurrence, occurrenceOf(topicId).getId());
   given().user().togglesSelectionOf(SelectedType::Association, associationId);
   given().user().linksTheSelection();
   Identifier roleId = roleOf(topicId, associationId).getId();
   given().user().selects(SelectedType::Role, roleId);
   when().user().setsTypeOfSelectionTo(typeTopicId);
   then().view().ofMap().shouldHaveTopicThat(topicId, [roleId](Topic const &topic) {
      Role const &role = *topic.findRoles(Identifiers::ofSingle(roleId)).begin();
      auto optionalType = role.getType();
      ASSERT_FALSE(optionalType.has_value());
   });
}

TEST_P(EditorTest, movingSelection)
{
   auto position = someSpacialCoordinate();
   auto offset = SpacialCoordinate::Offset::of(10.0f, 20.0f);
   Identifier topicId = given().user().requestsANewTopic().at(position);
   Identifier occurrenceId = occurrenceOf(topicId).getId();
   given().user().selects(SelectedType::Occurrence, occurrenceId);
   when().user().movesTheSelectionBy(offset);
   then().view().ofMap().shouldHaveTopicThat(topicId, [occurrenceId, position, offset](Topic const &topic) {
      auto occurrence = topic.getOccurrence(occurrenceId);
      ASSERT_TRUE(occurrence.has_value());
      auto expected = position.getAbsoluteReference().plus(offset);
      EXPECT_THAT(occurrence->get().getLocation().getSpacial(), isCloseTo(SpacialCoordinate::absoluteAt(expected.X(), expected.Y())));
   });
}
