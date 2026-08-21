// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <optional>
#include <string>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "Robot.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/simulation/DIOSim.hpp"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/units/time.hpp"

class DigitalCommunicationTest {
 public:
  wpi::sim::DIOSim allianceOutput{Robot::ALLIANCE_PORT};
  wpi::sim::DIOSim enabledOutput{Robot::ENABLED_PORT};
  wpi::sim::DIOSim autonomousOutput{Robot::AUTONOMOUS_PORT};
  wpi::sim::DIOSim alertOutput{Robot::ALERT_PORT};
  Robot robot;
  std::optional<std::thread> thread;

  DigitalCommunicationTest() {
    wpi::sim::PauseTiming();
    wpi::sim::SetProgramStarted(false);
    wpi::sim::DriverStationSim::ResetData();

    thread = std::thread([&] { robot.StartCompetition(); });
    wpi::sim::WaitForProgramStart();
  }

  ~DigitalCommunicationTest() {
    robot.EndCompetition();
    thread->join();
    allianceOutput.ResetData();
    enabledOutput.ResetData();
    autonomousOutput.ResetData();
    alertOutput.ResetData();
  }
};

TEST_CASE_METHOD(DigitalCommunicationTest, "DigitalCommunication alliance",
                 "[wpilibcExamples][snippets][dio]") {
  auto alliance = GENERATE(
      wpi::hal::AllianceStationID::RED_1, wpi::hal::AllianceStationID::RED_2,
      wpi::hal::AllianceStationID::RED_3, wpi::hal::AllianceStationID::BLUE_1,
      wpi::hal::AllianceStationID::BLUE_2, wpi::hal::AllianceStationID::BLUE_3,
      wpi::hal::AllianceStationID::UNKNOWN);
  wpi::sim::DriverStationSim::SetAllianceStationId(alliance);
  wpi::sim::DriverStationSim::NotifyNewData();

  CHECK(allianceOutput.GetInitialized());
  CHECK_FALSE(allianceOutput.GetIsInput());

  wpi::sim::StepTiming(20_ms);

  bool isRed = false;
  switch (alliance) {
    case wpi::hal::AllianceStationID::BLUE_1:
    case wpi::hal::AllianceStationID::BLUE_2:
    case wpi::hal::AllianceStationID::BLUE_3:
    case wpi::hal::AllianceStationID::UNKNOWN:
      isRed = false;
      break;
    case wpi::hal::AllianceStationID::RED_1:
    case wpi::hal::AllianceStationID::RED_2:
    case wpi::hal::AllianceStationID::RED_3:
      isRed = true;
      break;
  }
  CHECK(isRed == allianceOutput.GetValue());
}

TEST_CASE_METHOD(DigitalCommunicationTest, "DigitalCommunication enabled",
                 "[wpilibcExamples][snippets][dio]") {
  auto enabled = GENERATE(false, true);
  wpi::sim::DriverStationSim::SetEnabled(enabled);
  wpi::sim::DriverStationSim::NotifyNewData();

  CHECK(enabledOutput.GetInitialized());
  CHECK_FALSE(enabledOutput.GetIsInput());

  wpi::sim::StepTiming(20_ms);

  CHECK(enabled == enabledOutput.GetValue());
}

TEST_CASE_METHOD(DigitalCommunicationTest, "DigitalCommunication autonomous",
                 "[wpilibcExamples][snippets][dio]") {
  auto autonomous = GENERATE(false, true);
  wpi::sim::DriverStationSim::SetRobotMode(
      autonomous ? wpi::hal::RobotMode::AUTONOMOUS
                 : wpi::hal::RobotMode::TELEOPERATED);
  wpi::sim::DriverStationSim::NotifyNewData();

  CHECK(autonomousOutput.GetInitialized());
  CHECK_FALSE(autonomousOutput.GetIsInput());

  wpi::sim::StepTiming(20_ms);

  CHECK(autonomous == autonomousOutput.GetValue());
}

TEST_CASE_METHOD(DigitalCommunicationTest, "DigitalCommunication alert",
                 "[wpilibcExamples][snippets][dio]") {
  auto matchTime = GENERATE(45.0, 27.0, 23.0);
  wpi::sim::DriverStationSim::SetMatchTime(matchTime);
  wpi::sim::DriverStationSim::NotifyNewData();

  CHECK(alertOutput.GetInitialized());
  CHECK_FALSE(alertOutput.GetIsInput());

  wpi::sim::StepTiming(20_ms);

  CHECK((matchTime <= 30 && matchTime >= 25) == alertOutput.GetValue());
}
