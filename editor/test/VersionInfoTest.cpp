#include <gtest/gtest.h>

#include "contomap/editor/VersionInfo.h"

using contomap::editor::VersionInfo;

TEST(VersionInfoTest, humanReadableVariants)
{
   EXPECT_EQ("branch/name@1234", VersionInfo("branch/name", "1234", "").humanReadable());
   EXPECT_EQ("tag", VersionInfo("branch/name", "1234", "tag").humanReadable());
}

TEST(VersionInfoTest, globalConstantIsSet)
{
   EXPECT_NE("@GIT_TAG@", VersionInfo::GLOBAL.humanReadable()) << "CMake needs to create the necessary file. See CMakeLists.txt";
}
