#include <array>
#include <variant>

#include <gmock/gmock.h>

#include "contomap/infrastructure/LinkedReferences.h"

using contomap::infrastructure::LinkedReferences;
using contomap::infrastructure::Referable;

class LinkedReferencesTest : public testing::Test
{
public:
   class TestingReferable : public Referable<TestingReferable>
   {
   public:
      TestingReferable &refine() override
      {
         return *this;
      }
   };
   class TestingReferableAlt : public Referable<TestingReferableAlt>
   {
   public:
      TestingReferableAlt &refine() override
      {
         return *this;
      }
   };
};

TEST_F(LinkedReferencesTest, referablesCanBeTracked)
{
   auto ref = std::make_unique<TestingReferable>();
   LinkedReferences<TestingReferable> set;

   EXPECT_FALSE(set.contains(*ref));
   set.add(*ref);
   set.add(*ref);
   EXPECT_TRUE(set.contains(*ref));
   ref.reset();
   EXPECT_TRUE(set.empty());
}

TEST_F(LinkedReferencesTest, referablesCanBeRemoved)
{
   TestingReferable ref;
   LinkedReferences<TestingReferable> set;

   set.add(ref);
   EXPECT_TRUE(set.contains(ref));
   set.remove(ref);
   EXPECT_TRUE(set.empty());
}

TEST_F(LinkedReferencesTest, referablesCanBeIteratedOver)
{
   std::array<TestingReferable, 3> refs;
   LinkedReferences<TestingReferable> set;

   for (auto &ref : refs)
   {
      set.add(ref);
   }
   for (auto &ref : set.allReferences())
   {
      EXPECT_TRUE(std::find_if(refs.begin(), refs.end(), [&ref](TestingReferable const &entry) { return &entry == &ref.get(); }) != refs.end());
   }
}

TEST_F(LinkedReferencesTest, referencesContainsOther)
{
   std::array<TestingReferable, 4> refs;
   LinkedReferences<TestingReferable> set1;
   LinkedReferences<TestingReferable> set2;
   LinkedReferences<TestingReferable> set3;
   LinkedReferences<TestingReferable> setEmpty;

   set1.add(refs[1]);
   set1.add(refs[2]);
   set2.add(refs[0]);
   set2.add(refs[1]);
   set2.add(refs[2]);
   set3.add(refs[0]);
   set3.add(refs[3]);

   EXPECT_TRUE(set1.contains(set1)) << "set should contain itself";
   EXPECT_TRUE(set2.contains(set1)) << "larger set should contain smaller";
   EXPECT_FALSE(set1.contains(set2)) << "smaller should not contain larger";
   EXPECT_FALSE(set3.contains(set1)) << "distinct set should not contain other A";
   EXPECT_FALSE(set1.contains(set3)) << "distinct set should not contain other B";
   EXPECT_FALSE(set3.contains(set2)) << "partial set should not contain other A";
   EXPECT_FALSE(set2.contains(set3)) << "partial set should not contain other B";
   EXPECT_FALSE(setEmpty.contains(set1)) << "empty set should not contain other";
   EXPECT_TRUE(set1.contains(setEmpty)) << "set should contain empty";
}

TEST_F(LinkedReferencesTest, assignment)
{
   std::array<TestingReferable, 4> refs;
   LinkedReferences<TestingReferable> set1;
   set1.add(refs[0]);
   set1.add(refs[1]);
   LinkedReferences<TestingReferable> set2;
   set2.add(refs[2]);
   set2.add(refs[3]);
   LinkedReferences<TestingReferable> set3(set1);
   EXPECT_TRUE(set1.contains(set3) && set3.contains(set1)) << "copy constructor should work";
   set3 = set2;
   EXPECT_TRUE(set2.contains(set3) && set3.contains(set2)) << "copy operator should work";
}

TEST_F(LinkedReferencesTest, iteratorConstructor)
{
   std::array<TestingReferable, 4> refs;
   LinkedReferences<TestingReferable> set1;
   set1.add(refs[0]);
   set1.add(refs[1]);
   auto refView = std::ranges::common_view { set1.allReferences() };
   LinkedReferences<TestingReferable> set2(refView.begin(), refView.end());
   EXPECT_TRUE(set1.contains(set2) && set2.contains(set1));
}

TEST_F(LinkedReferencesTest, variantToReferences)
{
   using Variants = std::variant<LinkedReferences<TestingReferable>, LinkedReferences<TestingReferableAlt>>;
   Variants v;
   LinkedReferences<TestingReferable> a;
   LinkedReferences<TestingReferableAlt> b;
   v = a;
   std::optional<std::reference_wrapper<contomap::infrastructure::References>> referencesA;
   std::visit([&referencesA](contomap::infrastructure::References &references) { referencesA = references; }, v);
   EXPECT_TRUE(referencesA.has_value());
   v = b;
   std::optional<std::reference_wrapper<contomap::infrastructure::References>> referencesB;
   std::visit([&referencesB](contomap::infrastructure::References &references) { referencesB = references; }, v);
   EXPECT_TRUE(referencesB.has_value());
}
