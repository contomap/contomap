#include <gtest/gtest.h>

#include "contomap/infrastructure/Link.h"

using contomap::infrastructure::Link;
using contomap::infrastructure::Links;

TEST(LinkTest, linkBehaviour)
{
   size_t aUnlinks = 0;
   size_t bUnlinks = 0;
   auto aUnlinked = [&aUnlinks]() { aUnlinks++; };
   auto bUnlinked = [&bUnlinks]() { bUnlinks++; };
   int a = 0;
   float b = 0.0f;
   auto links = Links::between(a, aUnlinked, b, bUnlinked);
   EXPECT_EQ(aUnlinks, 0) << "should not unlink A during construction";
   EXPECT_EQ(bUnlinks, 0) << "should not unlink B during construction";

   EXPECT_EQ(&a, &links.first->getLinked()) << "should provide reference to A";
   EXPECT_EQ(&b, &links.second->getLinked()) << "should provide reference to B";

   links.first.reset();
   EXPECT_EQ(aUnlinks, 0) << "should not unlink A during destruction of A";
   EXPECT_EQ(bUnlinks, 1) << "should unlink B during destruction of A";

   links.second.reset();
   EXPECT_EQ(aUnlinks, 0) << "should not unlink A during destruction of unlinked B";
   EXPECT_EQ(bUnlinks, 1) << "should not unlink B during destruction of B";
}
