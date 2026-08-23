// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/TimedRobot.hpp"

#include <stdint.h>

#include <atomic>
#include <string>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/hal/DriverStationTypes.h"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/simulation/AlertSim.hpp"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/tunables/Tunable.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/Tunables.hpp"
#include "wpi/util/timestamp.hpp"

using namespace wpi;

inline constexpr auto PERIOD = 20_ms;

namespace {
class TimedRobotTest {
 public:
  TimedRobotTest() {
    wpi::telemetry::TelemetryRegistry::Reset();
    wpi::tunables::TunableRegistry::Reset();
    wpi::sim::AlertSim::ResetData();
    wpi::sim::PauseTiming();
    wpi::sim::SetProgramStarted(false);
  }

  ~TimedRobotTest() {
    wpi::telemetry::TelemetryRegistry::Reset();
    wpi::tunables::TunableRegistry::Reset();
    wpi::sim::AlertSim::ResetData();
    wpi::sim::ResumeTiming();
    wpi::nt::ResetInstance(wpi::nt::GetDefaultInstance());
  }
};

class MockRobot : public TimedRobot {
 public:
  std::atomic<uint32_t> m_simulationInitCount{0};
  std::atomic<uint32_t> m_disabledInitCount{0};
  std::atomic<uint32_t> m_autonomousInitCount{0};
  std::atomic<uint32_t> m_teleopInitCount{0};
  std::atomic<uint32_t> m_utilityInitCount{0};

  std::atomic<uint32_t> m_disabledExitCount{0};
  std::atomic<uint32_t> m_autonomousExitCount{0};
  std::atomic<uint32_t> m_teleopExitCount{0};
  std::atomic<uint32_t> m_utilityExitCount{0};

  std::atomic<uint32_t> m_robotPeriodicCount{0};
  std::atomic<uint32_t> m_simulationPeriodicCount{0};
  std::atomic<uint32_t> m_disabledPeriodicCount{0};
  std::atomic<uint32_t> m_autonomousPeriodicCount{0};
  std::atomic<uint32_t> m_teleopPeriodicCount{0};
  std::atomic<uint32_t> m_utilityPeriodicCount{0};

  MockRobot() : TimedRobot{PERIOD} {}

  void SimulationInit() override { m_simulationInitCount++; }

  void DisabledInit() override { m_disabledInitCount++; }

  void AutonomousInit() override { m_autonomousInitCount++; }

  void TeleopInit() override { m_teleopInitCount++; }

  void UtilityInit() override { m_utilityInitCount++; }

  void RobotPeriodic() override { m_robotPeriodicCount++; }

  void SimulationPeriodic() override { m_simulationPeriodicCount++; }

  void DisabledPeriodic() override { m_disabledPeriodicCount++; }

  void AutonomousPeriodic() override { m_autonomousPeriodicCount++; }

  void TeleopPeriodic() override { m_teleopPeriodicCount++; }

  void UtilityPeriodic() override { m_utilityPeriodicCount++; }

  void DisabledExit() override { m_disabledExitCount++; }

  void AutonomousExit() override { m_autonomousExitCount++; }

  void TeleopExit() override { m_teleopExitCount++; }

  void UtilityExit() override { m_utilityExitCount++; }
};
}  // namespace

TEST_CASE_METHOD(TimedRobotTest, "TimedRobotTest DisabledMode", "[wpilibc]") {
  MockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();

  CHECK(1u == robot.m_simulationInitCount);
  CHECK(0u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_autonomousInitCount);
  CHECK(0u == robot.m_teleopInitCount);
  CHECK(0u == robot.m_utilityInitCount);

  CHECK(0u == robot.m_robotPeriodicCount);
  CHECK(0u == robot.m_simulationPeriodicCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(0u == robot.m_autonomousPeriodicCount);
  CHECK(0u == robot.m_teleopPeriodicCount);
  CHECK(0u == robot.m_utilityPeriodicCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  wpi::sim::StepTiming(PERIOD);

  CHECK(1u == robot.m_simulationInitCount);
  CHECK(1u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_autonomousInitCount);
  CHECK(0u == robot.m_teleopInitCount);
  CHECK(0u == robot.m_utilityInitCount);

  CHECK(1u == robot.m_robotPeriodicCount);
  CHECK(1u == robot.m_simulationPeriodicCount);
  CHECK(1u == robot.m_disabledPeriodicCount);
  CHECK(0u == robot.m_autonomousPeriodicCount);
  CHECK(0u == robot.m_teleopPeriodicCount);
  CHECK(0u == robot.m_utilityPeriodicCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  wpi::sim::StepTiming(PERIOD);

  CHECK(1u == robot.m_simulationInitCount);
  CHECK(1u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_autonomousInitCount);
  CHECK(0u == robot.m_teleopInitCount);
  CHECK(0u == robot.m_utilityInitCount);

  CHECK(2u == robot.m_robotPeriodicCount);
  CHECK(2u == robot.m_simulationPeriodicCount);
  CHECK(2u == robot.m_disabledPeriodicCount);
  CHECK(0u == robot.m_autonomousPeriodicCount);
  CHECK(0u == robot.m_teleopPeriodicCount);
  CHECK(0u == robot.m_utilityPeriodicCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_CASE_METHOD(TimedRobotTest,
                 "TimedRobotTest ConstructorPublishesProgramStartTime",
                 "[wpilibc]") {
  auto sub = wpi::nt::NetworkTableInstance::GetDefault()
                 .GetIntegerTopic("/Robot/ProgramStartTime")
                 .Subscribe(-1);
  MockRobot robot;

  CHECK(static_cast<int64_t>(wpi::util::GetProgramStartTime()) == sub.Get(-1));
}

TEST_CASE_METHOD(
    TimedRobotTest,
    "TimedRobotTest ConstructorRegistersTelemetryAndTunableBackends",
    "[wpilibc]") {
  auto inst = wpi::nt::NetworkTableInstance::GetDefault();
  MockRobot robot;

  wpi::telemetry::Log("telemetryDouble", 2.5);

  auto telemetrySub =
      inst.GetDoubleTopic("/Telemetry/telemetryDouble").Subscribe(0.0);
  CHECK(telemetrySub.Get() == 2.5);

  wpi::tunables::TunableDouble tunable{1.0};
  wpi::tunables::Publish("tunableDouble", tunable);

  auto tunableSub =
      inst.GetDoubleTopic("/Tunables/tunableDouble").Subscribe(0.0);
  CHECK(tunableSub.Get() == 1.0);

  auto tunablePub = inst.GetDoubleTopic("/Tunables/tunableDouble").Publish();
  tunablePub.Set(3.5);
  wpi::tunables::TunableRegistry::Update();

  CHECK(tunable.Get() == 3.5);
}

TEST_CASE_METHOD(TimedRobotTest,
                 "TimedRobotTest ConstructorMapsWarningsToAlerts",
                 "[wpilibc]") {
  {
    MockRobot robot;

    wpi::telemetry::TelemetryRegistry::ReportWarning("/bad",
                                                     "telemetry test warning");
    wpi::tunables::TunableRegistry::ReportWarning("tunable test warning");

    auto alerts = wpi::sim::AlertSim::GetActive();
    CHECK(alerts.size() == 2);

    bool sawTelemetry = false;
    bool sawTunable = false;
    for (const auto& alert : alerts) {
      if (alert.group == "Telemetry" &&
          alert.text.find("telemetry test warning") != std::string::npos) {
        sawTelemetry = true;
      }
      if (alert.group == "Tunables" &&
          alert.text.find("tunable test warning") != std::string::npos) {
        sawTunable = true;
      }
    }
    CHECK(sawTelemetry);
    CHECK(sawTunable);
  }

  CHECK(wpi::sim::AlertSim::GetActive().empty());
}

TEST_CASE_METHOD(TimedRobotTest, "TimedRobotTest AutonomousMode", "[wpilibc]") {
  MockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::SetRobotMode(hal::RobotMode::AUTONOMOUS);
  wpi::sim::DriverStationSim::NotifyNewData();

  CHECK(1u == robot.m_simulationInitCount);
  CHECK(0u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_autonomousInitCount);
  CHECK(0u == robot.m_teleopInitCount);
  CHECK(0u == robot.m_utilityInitCount);

  CHECK(0u == robot.m_robotPeriodicCount);
  CHECK(0u == robot.m_simulationPeriodicCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(0u == robot.m_autonomousPeriodicCount);
  CHECK(0u == robot.m_teleopPeriodicCount);
  CHECK(0u == robot.m_utilityPeriodicCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  wpi::sim::StepTiming(PERIOD);

  CHECK(1u == robot.m_simulationInitCount);
  CHECK(0u == robot.m_disabledInitCount);
  CHECK(1u == robot.m_autonomousInitCount);
  CHECK(0u == robot.m_teleopInitCount);
  CHECK(0u == robot.m_utilityInitCount);

  CHECK(1u == robot.m_robotPeriodicCount);
  CHECK(1u == robot.m_simulationPeriodicCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(1u == robot.m_autonomousPeriodicCount);
  CHECK(0u == robot.m_teleopPeriodicCount);
  CHECK(0u == robot.m_utilityPeriodicCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  wpi::sim::StepTiming(PERIOD);

  CHECK(1u == robot.m_simulationInitCount);
  CHECK(0u == robot.m_disabledInitCount);
  CHECK(1u == robot.m_autonomousInitCount);
  CHECK(0u == robot.m_teleopInitCount);
  CHECK(0u == robot.m_utilityInitCount);

  CHECK(2u == robot.m_robotPeriodicCount);
  CHECK(2u == robot.m_simulationPeriodicCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(2u == robot.m_autonomousPeriodicCount);
  CHECK(0u == robot.m_teleopPeriodicCount);
  CHECK(0u == robot.m_utilityPeriodicCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_CASE_METHOD(TimedRobotTest, "TimedRobotTest TeleopMode", "[wpilibc]") {
  MockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::SetRobotMode(hal::RobotMode::TELEOPERATED);
  wpi::sim::DriverStationSim::NotifyNewData();

  CHECK(1u == robot.m_simulationInitCount);
  CHECK(0u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_autonomousInitCount);
  CHECK(0u == robot.m_teleopInitCount);
  CHECK(0u == robot.m_utilityInitCount);

  CHECK(0u == robot.m_robotPeriodicCount);
  CHECK(0u == robot.m_simulationPeriodicCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(0u == robot.m_autonomousPeriodicCount);
  CHECK(0u == robot.m_teleopPeriodicCount);
  CHECK(0u == robot.m_utilityPeriodicCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  wpi::sim::StepTiming(PERIOD);

  CHECK(1u == robot.m_simulationInitCount);
  CHECK(0u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_autonomousInitCount);
  CHECK(1u == robot.m_teleopInitCount);
  CHECK(0u == robot.m_utilityInitCount);

  CHECK(1u == robot.m_robotPeriodicCount);
  CHECK(1u == robot.m_simulationPeriodicCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(0u == robot.m_autonomousPeriodicCount);
  CHECK(1u == robot.m_teleopPeriodicCount);
  CHECK(0u == robot.m_utilityPeriodicCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  wpi::sim::StepTiming(PERIOD);

  CHECK(1u == robot.m_simulationInitCount);
  CHECK(0u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_autonomousInitCount);
  CHECK(1u == robot.m_teleopInitCount);
  CHECK(0u == robot.m_utilityInitCount);

  CHECK(2u == robot.m_robotPeriodicCount);
  CHECK(2u == robot.m_simulationPeriodicCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(0u == robot.m_autonomousPeriodicCount);
  CHECK(2u == robot.m_teleopPeriodicCount);
  CHECK(0u == robot.m_utilityPeriodicCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_CASE_METHOD(TimedRobotTest, "TimedRobotTest UtilityMode", "[wpilibc]") {
  MockRobot robot;
  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::SetRobotMode(hal::RobotMode::UTILITY);
  wpi::sim::DriverStationSim::NotifyNewData();

  CHECK(1u == robot.m_simulationInitCount);
  CHECK(0u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_autonomousInitCount);
  CHECK(0u == robot.m_teleopInitCount);
  CHECK(0u == robot.m_utilityInitCount);

  CHECK(0u == robot.m_robotPeriodicCount);
  CHECK(0u == robot.m_simulationPeriodicCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(0u == robot.m_autonomousPeriodicCount);
  CHECK(0u == robot.m_teleopPeriodicCount);
  CHECK(0u == robot.m_utilityPeriodicCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  wpi::sim::StepTiming(PERIOD);

  CHECK(1u == robot.m_simulationInitCount);
  CHECK(0u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_autonomousInitCount);
  CHECK(0u == robot.m_teleopInitCount);
  CHECK(1u == robot.m_utilityInitCount);

  CHECK(1u == robot.m_robotPeriodicCount);
  CHECK(1u == robot.m_simulationPeriodicCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(0u == robot.m_autonomousPeriodicCount);
  CHECK(0u == robot.m_teleopPeriodicCount);
  CHECK(1u == robot.m_utilityPeriodicCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  wpi::sim::StepTiming(PERIOD);

  CHECK(1u == robot.m_simulationInitCount);
  CHECK(0u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_autonomousInitCount);
  CHECK(0u == robot.m_teleopInitCount);
  CHECK(1u == robot.m_utilityInitCount);

  CHECK(2u == robot.m_robotPeriodicCount);
  CHECK(2u == robot.m_simulationPeriodicCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(0u == robot.m_autonomousPeriodicCount);
  CHECK(0u == robot.m_teleopPeriodicCount);
  CHECK(2u == robot.m_utilityPeriodicCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(20_ms);  // Wait for Notifiers

  CHECK(1u == robot.m_simulationInitCount);
  CHECK(1u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_autonomousInitCount);
  CHECK(0u == robot.m_teleopInitCount);
  CHECK(1u == robot.m_utilityInitCount);

  CHECK(3u == robot.m_robotPeriodicCount);
  CHECK(3u == robot.m_simulationPeriodicCount);
  CHECK(1u == robot.m_disabledPeriodicCount);
  CHECK(0u == robot.m_autonomousPeriodicCount);
  CHECK(0u == robot.m_teleopPeriodicCount);
  CHECK(2u == robot.m_utilityPeriodicCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(1u == robot.m_utilityExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_CASE_METHOD(TimedRobotTest, "TimedRobotTest ModeChange", "[wpilibc]") {
  MockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  // Start in disabled
  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();

  CHECK(0u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_autonomousInitCount);
  CHECK(0u == robot.m_teleopInitCount);
  CHECK(0u == robot.m_utilityInitCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  wpi::sim::StepTiming(PERIOD);

  CHECK(1u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_autonomousInitCount);
  CHECK(0u == robot.m_teleopInitCount);
  CHECK(0u == robot.m_utilityInitCount);

  CHECK(0u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  // Transition to autonomous
  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::SetRobotMode(hal::RobotMode::AUTONOMOUS);
  wpi::sim::DriverStationSim::NotifyNewData();

  wpi::sim::StepTiming(PERIOD);

  CHECK(1u == robot.m_disabledInitCount);
  CHECK(1u == robot.m_autonomousInitCount);
  CHECK(0u == robot.m_teleopInitCount);
  CHECK(0u == robot.m_utilityInitCount);

  CHECK(1u == robot.m_disabledExitCount);
  CHECK(0u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  // Transition to teleop
  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::SetRobotMode(hal::RobotMode::TELEOPERATED);
  wpi::sim::DriverStationSim::NotifyNewData();

  wpi::sim::StepTiming(PERIOD);

  CHECK(1u == robot.m_disabledInitCount);
  CHECK(1u == robot.m_autonomousInitCount);
  CHECK(1u == robot.m_teleopInitCount);
  CHECK(0u == robot.m_utilityInitCount);

  CHECK(1u == robot.m_disabledExitCount);
  CHECK(1u == robot.m_autonomousExitCount);
  CHECK(0u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  // Transition to utility
  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::SetRobotMode(hal::RobotMode::UTILITY);
  wpi::sim::DriverStationSim::NotifyNewData();

  wpi::sim::StepTiming(PERIOD);

  CHECK(1u == robot.m_disabledInitCount);
  CHECK(1u == robot.m_autonomousInitCount);
  CHECK(1u == robot.m_teleopInitCount);
  CHECK(1u == robot.m_utilityInitCount);

  CHECK(1u == robot.m_disabledExitCount);
  CHECK(1u == robot.m_autonomousExitCount);
  CHECK(1u == robot.m_teleopExitCount);
  CHECK(0u == robot.m_utilityExitCount);

  // Transition to disabled
  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();

  wpi::sim::StepTiming(PERIOD);

  CHECK(2u == robot.m_disabledInitCount);
  CHECK(1u == robot.m_autonomousInitCount);
  CHECK(1u == robot.m_teleopInitCount);
  CHECK(1u == robot.m_utilityInitCount);

  CHECK(1u == robot.m_disabledExitCount);
  CHECK(1u == robot.m_autonomousExitCount);
  CHECK(1u == robot.m_teleopExitCount);
  CHECK(1u == robot.m_utilityExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_CASE_METHOD(TimedRobotTest, "TimedRobotTest AddPeriodic", "[wpilibc]") {
  MockRobot robot;

  std::atomic<uint32_t> callbackCount{0};
  robot.AddPeriodic([&] { callbackCount++; }, PERIOD / 2.0);

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();

  CHECK(0u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(0u == callbackCount);

  wpi::sim::StepTiming(PERIOD / 2.0);

  CHECK(0u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(1u == callbackCount);

  wpi::sim::StepTiming(PERIOD / 2.0);

  CHECK(1u == robot.m_disabledInitCount);
  CHECK(1u == robot.m_disabledPeriodicCount);
  CHECK(2u == callbackCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_CASE_METHOD(TimedRobotTest, "TimedRobotTest AddPeriodicWithOffset",
                 "[wpilibc]") {
  MockRobot robot;

  std::atomic<uint32_t> callbackCount{0};
  robot.AddPeriodic([&] { callbackCount++; }, PERIOD / 2.0, PERIOD / 4.0);

  // Expirations in this test (ms)
  //
  // Robot | Callback
  // ================
  //     p |    0.75p
  //    2p |    1.25p

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();

  CHECK(0u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(0u == callbackCount);

  wpi::sim::StepTiming(PERIOD * 3.0 / 8.0);

  CHECK(0u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(0u == callbackCount);

  wpi::sim::StepTiming(PERIOD * 3.0 / 8.0);

  CHECK(0u == robot.m_disabledInitCount);
  CHECK(0u == robot.m_disabledPeriodicCount);
  CHECK(1u == callbackCount);

  wpi::sim::StepTiming(PERIOD / 4.0);

  CHECK(1u == robot.m_disabledInitCount);
  CHECK(1u == robot.m_disabledPeriodicCount);
  CHECK(1u == callbackCount);

  wpi::sim::StepTiming(PERIOD / 4.0);

  CHECK(1u == robot.m_disabledInitCount);
  CHECK(1u == robot.m_disabledPeriodicCount);
  CHECK(2u == callbackCount);

  robot.EndCompetition();
  robotThread.join();
}
