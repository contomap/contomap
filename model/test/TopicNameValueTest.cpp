#include <gtest/gtest.h>

#include "contomap/model/TopicNameValue.h"

using contomap::model::TopicNameValue;

TEST(TopicNameValueTest, fromFailures)
{
   EXPECT_EQ(0, TopicNameValue::from("").index()) << "empty name is not allowed";
   std::array<char, TopicNameValue::maxUtf8Bytes() + 2> tooLongName {};
   tooLongName.fill('a');
   tooLongName[tooLongName.size() - 1] = 0x00;
   EXPECT_EQ(0, TopicNameValue::from(tooLongName.data()).index()) << "name beyond length limit is not allowed";
}

TEST(TopicNameValueTest, comparison)
{
   EXPECT_EQ(std::get<TopicNameValue>(TopicNameValue::from("a")), std::get<TopicNameValue>(TopicNameValue::from("a")));
   EXPECT_NE(std::get<TopicNameValue>(TopicNameValue::from("b")), std::get<TopicNameValue>(TopicNameValue::from("a")));
   EXPECT_LT(std::get<TopicNameValue>(TopicNameValue::from("a")), std::get<TopicNameValue>(TopicNameValue::from("b")));
   EXPECT_GT(std::get<TopicNameValue>(TopicNameValue::from("b")), std::get<TopicNameValue>(TopicNameValue::from("a")));
}

TEST(TopicNameValueTest, raw)
{
   EXPECT_EQ("a", std::get<TopicNameValue>(TopicNameValue::from("a")).raw());
   EXPECT_EQ("b", std::get<TopicNameValue>(TopicNameValue::from("b")).raw());
}
