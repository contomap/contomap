#include <gtest/gtest.h>

#include "contomap/model/TopicName.h"

using contomap::model::TopicName;

TEST(TopicNameTest, fromFailures)
{
   EXPECT_EQ(0, TopicName::from("").index()) << "empty name is not allowed";
   std::array<char, TopicName::maxUtf8Bytes() + 2> tooLongName {};
   tooLongName.fill('a');
   tooLongName[tooLongName.size() - 1] = 0x00;
   EXPECT_EQ(0, TopicName::from(tooLongName.data()).index()) << "name beyond length limit is not allowed";
}

TEST(TopicNameTest, comparison)
{
   EXPECT_EQ(std::get<TopicName>(TopicName::from("a")), std::get<TopicName>(TopicName::from("a")));
   EXPECT_NE(std::get<TopicName>(TopicName::from("b")), std::get<TopicName>(TopicName::from("a")));
   EXPECT_LT(std::get<TopicName>(TopicName::from("a")), std::get<TopicName>(TopicName::from("b")));
   EXPECT_GT(std::get<TopicName>(TopicName::from("b")), std::get<TopicName>(TopicName::from("a")));
}
