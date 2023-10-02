#include <gmock/gmock.h>

#include "contomap/editor/Editor.h"

#include "contomap/test/fixtures/ContomapViewFixture.h"
#include "contomap/test/matchers/Coordinates.h"
#include "contomap/test/matchers/Topics.h"
#include "contomap/test/printers/model.h"
#include "contomap/test/samples/CoordinateSamples.h"
#include "contomap/test/samples/TopicNameSamples.h"
#include "contomap/test/Steps.h"

using contomap::editor::Editor;
using contomap::editor::InputRequestHandler;
using contomap::model::Identifier;
using contomap::model::Identifiers;
using contomap::model::Occurrence;
using contomap::model::SpacialCoordinate;
using contomap::model::TopicNameValue;

using contomap::test::fixtures::ContomapViewFixture;
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

   private:
      InputRequestHandler &handler;
   };

   class ViewFixture
   {
   public:
      explicit ViewFixture(contomap::editor::View &view)
         : view(view)
         , mapViewFixture(view.ofMap())
      {
      }

      ContomapViewFixture &ofMap()
      {
         return mapViewFixture;
      }

   private:
      contomap::editor::View &view;
      ContomapViewFixture mapViewFixture;
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
   then().view().ofMap().shouldHaveTopicThat(id, [this, &id, &name, &position](auto const &topic) {
      EXPECT_THAT(id, testing::Eq(topic.getId()));
      EXPECT_THAT(topic, hasName(name.raw()));
      std::vector<SpacialCoordinate> coordinates;
      std::ranges::copy(topic.occurrencesIn(viewScope()) | std::views::transform([](Occurrence const &o) { return o.getLocation().getSpacial(); }),
         std::back_inserter(coordinates));
      EXPECT_THAT(coordinates, testing::ElementsAre(isCloseTo(position)));
   });
}
