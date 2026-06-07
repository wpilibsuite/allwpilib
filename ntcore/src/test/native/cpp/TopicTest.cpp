// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/nt/Topic.hpp"

#include <gtest/gtest.h>

#include "wpi/nt/NetworkTableInstance.hpp"

class TopicTest : public ::testing::Test {
 public:
  TopicTest() : m_inst{wpi::nt::NetworkTableInstance::Create()} {}
  ~TopicTest() override { wpi::nt::NetworkTableInstance::Destroy(m_inst); }

 protected:
  wpi::nt::NetworkTableInstance m_inst;
};

TEST_F(TopicTest, UserDataDefaultsToNull) {
  auto topic = m_inst.GetTopic("foo");

  EXPECT_EQ(nullptr, topic.GetUserData());
}

TEST_F(TopicTest, UserDataRoundTrip) {
  auto topic = m_inst.GetTopic("foo");
  int data = 5;

  topic.SetUserData(&data);

  EXPECT_EQ(&data, topic.GetUserData());
}

TEST_F(TopicTest, UserDataCanBeReplacedAndCleared) {
  auto topic = m_inst.GetTopic("foo");
  auto sameTopic = m_inst.GetTopic("foo");
  int data1 = 5;
  int data2 = 10;

  topic.SetUserData(&data1);
  EXPECT_EQ(&data1, sameTopic.GetUserData());

  sameTopic.SetUserData(&data2);
  EXPECT_EQ(&data2, topic.GetUserData());

  topic.SetUserData(nullptr);
  EXPECT_EQ(nullptr, sameTopic.GetUserData());
}

TEST_F(TopicTest, UserDataInvalidTopic) {
  wpi::nt::Topic topic;
  int data = 5;

  EXPECT_EQ(nullptr, topic.GetUserData());

  topic.SetUserData(&data);

  EXPECT_EQ(nullptr, topic.GetUserData());
}
