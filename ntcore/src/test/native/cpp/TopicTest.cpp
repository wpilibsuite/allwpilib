// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/nt/Topic.hpp"

#include <catch2/catch_test_macros.hpp>

#include "wpi/nt/NetworkTableInstance.hpp"

class TopicTest {
 public:
  TopicTest() : m_inst{wpi::nt::NetworkTableInstance::Create()} {}
  ~TopicTest() { wpi::nt::NetworkTableInstance::Destroy(m_inst); }

 protected:
  wpi::nt::NetworkTableInstance m_inst;
};

TEST_CASE_METHOD(TopicTest, "TopicTest UserDataDefaultsToNull",
                 "[ntcore][topic]") {
  auto topic = m_inst.GetTopic("foo");

  CHECK(nullptr == topic.GetUserData());
}

TEST_CASE_METHOD(TopicTest, "TopicTest UserDataRoundTrip", "[ntcore][topic]") {
  auto topic = m_inst.GetTopic("foo");
  int data = 5;

  topic.SetUserData(&data);

  CHECK(&data == topic.GetUserData());
}

TEST_CASE_METHOD(TopicTest, "TopicTest UserDataCanBeReplacedAndCleared",
                 "[ntcore][topic]") {
  auto topic = m_inst.GetTopic("foo");
  auto sameTopic = m_inst.GetTopic("foo");
  int data1 = 5;
  int data2 = 10;

  topic.SetUserData(&data1);
  CHECK(&data1 == sameTopic.GetUserData());

  sameTopic.SetUserData(&data2);
  CHECK(&data2 == topic.GetUserData());

  topic.SetUserData(nullptr);
  CHECK(nullptr == sameTopic.GetUserData());
}

TEST_CASE_METHOD(TopicTest, "TopicTest UserDataInvalidTopic",
                 "[ntcore][topic]") {
  wpi::nt::Topic topic;
  int data = 5;

  CHECK(nullptr == topic.GetUserData());

  topic.SetUserData(&data);

  CHECK(nullptr == topic.GetUserData());
}
