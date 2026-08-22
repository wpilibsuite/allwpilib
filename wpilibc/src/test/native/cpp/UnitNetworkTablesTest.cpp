// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/UnitTopic.hpp"
#include "wpi/units/length.hpp"

class UnitNetworkTablesTest {
 public:
  UnitNetworkTablesTest() : inst{wpi::nt::NetworkTableInstance::Create()} {}
  ~UnitNetworkTablesTest() { wpi::nt::NetworkTableInstance::Destroy(inst); }
  wpi::nt::NetworkTableInstance inst;
};

TEST_CASE_METHOD(UnitNetworkTablesTest, "UnitNetworkTablesTest Publish",
                 "[wpilibc]") {
  auto topic =
      wpi::nt::UnitTopic<wpi::units::meters<>>{inst.GetTopic("meterTest")};
  auto pub = topic.Publish();
  pub.Set(2_m);
  REQUIRE(topic.GetProperty("unit") == "meters");
  REQUIRE(topic.IsMatchingUnit());
}

TEST_CASE_METHOD(UnitNetworkTablesTest, "UnitNetworkTablesTest SubscribeDouble",
                 "[wpilibc]") {
  auto topic =
      wpi::nt::UnitTopic<wpi::units::meters<>>{inst.GetTopic("meterTest")};
  auto pub = topic.Publish();
  auto sub = inst.GetDoubleTopic("meterTest").Subscribe(0);
  REQUIRE(sub.Get() == 0);
  REQUIRE(sub.Get(3) == 3);
  pub.Set(2_m);
  REQUIRE(sub.Get() == 2);
}

TEST_CASE_METHOD(UnitNetworkTablesTest, "UnitNetworkTablesTest SubscribeUnit",
                 "[wpilibc]") {
  auto topic =
      wpi::nt::UnitTopic<wpi::units::meters<>>{inst.GetTopic("meterTest")};
  auto pub = topic.Publish();
  auto sub = topic.Subscribe(0_m);
  REQUIRE(sub.Get() == 0_m);
  REQUIRE(sub.Get(3_m) == 3_m);
  pub.Set(2_m);
  REQUIRE(sub.Get() == 2_m);
}
