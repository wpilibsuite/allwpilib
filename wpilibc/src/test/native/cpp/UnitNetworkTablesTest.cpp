// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <networktables/DoubleTopic.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/UnitTopic.h>
#include <units/length.h>

#include "gtest/gtest.h"

class UnitNetworkTablesTest : public ::testing::Test {
 public:
  UnitNetworkTablesTest() : inst{nt::NetworkTableInstance::Create()} {}
  ~UnitNetworkTablesTest() override { nt::NetworkTableInstance::Destroy(inst); }
  nt::NetworkTableInstance inst;
};

TEST_F(UnitNetworkTablesTest, Publish) {
  auto topic = nt::UnitTopic<units::meter_t>{inst.GetTopic("meterTest")};
  auto pub = topic.Publish();
  pub.Set(2_m);
  ASSERT_EQ(topic.GetProperty("unit"), "meter");
  ASSERT_TRUE(topic.IsMatchingUnit());
}

TEST_F(UnitNetworkTablesTest, SubscribeDouble) {
  auto topic = nt::UnitTopic<units::meter_t>{inst.GetTopic("meterTest")};
  auto pub = topic.Publish();
  auto sub = inst.GetDoubleTopic("meterTest").Subscribe(0);
  ASSERT_EQ(sub.Get(), 0);
  ASSERT_EQ(sub.Get(3), 3);
  pub.Set(2_m);
  ASSERT_EQ(sub.Get(), 2);
}

TEST_F(UnitNetworkTablesTest, SubscribeUnit) {
  auto topic = nt::UnitTopic<units::meter_t>{inst.GetTopic("meterTest")};
  auto pub = topic.Publish();
  auto sub = topic.Subscribe(0_m);
  ASSERT_EQ(sub.Get(), 0_m);
  ASSERT_EQ(sub.Get(3_m), 3_m);
  pub.Set(2_m);
  ASSERT_EQ(sub.Get(), 2_m);
}
