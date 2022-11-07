// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <networktables/BooleanTopic.h>
#include <networktables/NetworkTableInstance.h>

#include "frc/event/EventLoop.h"
#include "frc/event/NetworkBooleanEvent.h"
#include "gtest/gtest.h"

using namespace frc;

class NetworkBooleanEventTest : public ::testing::Test {
 public:
  NetworkBooleanEventTest() {
    m_inst = nt::NetworkTableInstance::Create();
    m_inst.StartLocal();
  }

  ~NetworkBooleanEventTest() override {
    nt::NetworkTableInstance::Destroy(m_inst);
  }

  nt::NetworkTableInstance m_inst;
};

TEST_F(NetworkBooleanEventTest, Set) {
  EventLoop loop;
  int counter = 0;

  auto pub = m_inst.GetTable("TestTable")->GetBooleanTopic("Test").Publish();

  NetworkBooleanEvent(&loop, m_inst, "TestTable", "Test").IfHigh([&] {
    ++counter;
  });
  pub.Set(false);
  loop.Poll();
  EXPECT_EQ(0, counter);
  pub.Set(true);
  loop.Poll();
  EXPECT_EQ(1, counter);
  pub.Set(false);
  loop.Poll();
  EXPECT_EQ(1, counter);
}
