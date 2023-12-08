#include <gmock/gmock.h>

#include "contomap/model/Reifiable.h"
#include "contomap/model/Reifier.h"

using contomap::model::Identifier;
using contomap::model::Reifiable;
using contomap::model::Reified;
using contomap::model::Reifier;

using testing::NiceMock;

class ReifiableTest : public testing::Test
{
public:
   class MockedReifier : public Reifier<MockedReifier>
   {
   public:
      Identifier getId() const override
      {
         return Identifier::random();
      }

      MockedReifier &refine() override
      {
         return *this;
      }

      MOCK_METHOD1(setReified, void(Reified &));
      MOCK_METHOD0(clearReified, void());
   };

   class TestedReifiable : public Reifiable<MockedReifier>
   {
   public:
      ~TestedReifiable() override = default;
   };
};

TEST_F(ReifiableTest, reifierCanBeRetrieved)
{
   NiceMock<MockedReifier> reifier;
   TestedReifiable reifiable;
   EXPECT_FALSE(reifiable.hasReifier());
   EXPECT_FALSE(reifiable.getReifier().has_value());
   reifiable.setReifier(reifier);
   EXPECT_TRUE(reifiable.hasReifier());
   ASSERT_TRUE(reifiable.getReifier().has_value());
   EXPECT_EQ(&reifiable.getReifier().value().get(), &reifier);
}

TEST_F(ReifiableTest, settingReifierForwardsReified)
{
   NiceMock<MockedReifier> reifier;
   TestedReifiable reifiable;
   EXPECT_CALL(reifier, setReified(testing::Ref(reifiable)));
   reifiable.setReifier(reifier);
}

TEST_F(ReifiableTest, clearingReifierForwardsToReifier)
{
   NiceMock<MockedReifier> reifier;
   TestedReifiable reifiable;
   reifiable.setReifier(reifier);
   EXPECT_CALL(reifier, clearReified());
   reifiable.clearReifier();
}

TEST_F(ReifiableTest, settingNewReifierClearsPreviousReifier)
{
   NiceMock<MockedReifier> reifier1;
   NiceMock<MockedReifier> reifier2;
   TestedReifiable reifiable;
   reifiable.setReifier(reifier1);
   EXPECT_CALL(reifier1, clearReified());
   reifiable.setReifier(reifier2);
}

TEST_F(ReifiableTest, clearingReifierGuardsAgainstCallback)
{
   NiceMock<MockedReifier> reifier;
   TestedReifiable reifiable;
   reifiable.setReifier(reifier);
   size_t times = 0;
   ON_CALL(reifier, clearReified()).WillByDefault(testing::Invoke([&reifiable, &times]() {
      times++;
      if (times == 1)
      {
         reifiable.clearReifier();
      }
   }));
   reifiable.clearReifier();
   EXPECT_EQ(1, times);
}
