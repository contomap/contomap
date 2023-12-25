#include <array>

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
};

TEST_F(LinkedReferencesTest, referablesCanBeTracked)
{
   auto ref1 = std::make_unique<TestingReferable>();
   LinkedReferences<TestingReferable> set;

   EXPECT_FALSE(set.contains(*ref1));
   set.add(*ref1);
   set.add(*ref1);
   EXPECT_TRUE(set.contains(*ref1));
   ref1.reset();
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
