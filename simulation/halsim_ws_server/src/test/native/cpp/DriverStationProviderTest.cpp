// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/hal/simulation/DriverStationData.h"
#include "wpi/halsim/ws_core/WSProvider_DriverStation.hpp"

namespace wpilibws {
namespace {

class MockWebSocketConnection : public HALSimBaseWebSocketConnection {
 public:
  void OnSimValueChanged(const wpi::util::json& msg) override {
    messages.push_back(msg);
  }

  std::vector<wpi::util::json> messages;
};

struct DriverStationProviderTest {
  DriverStationProviderTest() { HALSIM_ResetDriverStationData(); }

  void CheckMode(HAL_RobotMode mode, bool autonomous, bool test) {
    bool found = false;
    for (const auto& msg : connection->messages) {
      const auto& data = msg.at("data");
      if (!data.contains(">robotMode")) {
        continue;
      }
      CHECK_FALSE(found);
      found = true;
      CHECK(msg.at("type").get_string() == "DriverStation");
      CHECK(msg.at("device").get_string().empty());
      CHECK(data.at(">robotMode").get_int() == static_cast<int>(mode));
      REQUIRE(data.contains(">autonomous"));
      CHECK(data.at(">autonomous").get_bool() == autonomous);
      REQUIRE(data.contains(">test"));
      CHECK(data.at(">test").get_bool() == test);
    }
    CHECK(found);
  }

  std::shared_ptr<MockWebSocketConnection> connection =
      std::make_shared<MockWebSocketConnection>();
  HALSimWSProviderDriverStation provider{"DriverStation", "DriverStation"};
};

}  // namespace

TEST_CASE_METHOD(DriverStationProviderTest,
                 "DriverStationProviderTest InitialMode",
                 "[simulation][halsim_ws_server]") {
  SECTION("Unknown") {
    provider.OnNetworkConnected(connection);
    CheckMode(HAL_ROBOT_MODE_UNKNOWN, false, false);
  }
  SECTION("Autonomous") {
    HALSIM_SetDriverStationRobotMode(HAL_ROBOT_MODE_AUTONOMOUS);
    provider.OnNetworkConnected(connection);
    CheckMode(HAL_ROBOT_MODE_AUTONOMOUS, true, false);
  }
  SECTION("Teleoperated") {
    HALSIM_SetDriverStationRobotMode(HAL_ROBOT_MODE_TELEOPERATED);
    provider.OnNetworkConnected(connection);
    CheckMode(HAL_ROBOT_MODE_TELEOPERATED, false, false);
  }
  SECTION("Utility") {
    HALSIM_SetDriverStationRobotMode(HAL_ROBOT_MODE_UTILITY);
    provider.OnNetworkConnected(connection);
    CheckMode(HAL_ROBOT_MODE_UTILITY, false, true);
  }
}

TEST_CASE_METHOD(DriverStationProviderTest,
                 "DriverStationProviderTest ModeTransitions",
                 "[simulation][halsim_ws_server]") {
  provider.OnNetworkConnected(connection);
  connection->messages.clear();

  HALSIM_SetDriverStationEnabled(true);
  HALSIM_SetDriverStationRobotMode(HAL_ROBOT_MODE_AUTONOMOUS);
  CheckMode(HAL_ROBOT_MODE_AUTONOMOUS, true, false);
  connection->messages.clear();

  // The Romi server must see the legacy flags when selecting teleop while
  // disabled, so it can clear its cached autonomous motor output.
  HALSIM_SetDriverStationEnabled(false);
  HALSIM_SetDriverStationRobotMode(HAL_ROBOT_MODE_TELEOPERATED);
  CheckMode(HAL_ROBOT_MODE_TELEOPERATED, false, false);
  connection->messages.clear();

  HALSIM_SetDriverStationEnabled(true);
  HALSIM_SetDriverStationRobotMode(HAL_ROBOT_MODE_UTILITY);
  CheckMode(HAL_ROBOT_MODE_UTILITY, false, true);
  connection->messages.clear();

  HALSIM_SetDriverStationRobotMode(HAL_ROBOT_MODE_UNKNOWN);
  CheckMode(HAL_ROBOT_MODE_UNKNOWN, false, false);
  connection->messages.clear();

  provider.OnNetworkDisconnected();
  HALSIM_SetDriverStationRobotMode(HAL_ROBOT_MODE_AUTONOMOUS);
  CHECK(connection->messages.empty());

  provider.OnNetworkConnected(connection);
  CheckMode(HAL_ROBOT_MODE_AUTONOMOUS, true, false);
}

}  // namespace wpilibws
