// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/OpModeRobot.hpp"

#include <sys/types.h>

#include <memory>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/driverstation/RobotState.hpp"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/util/Color.hpp"
#include "wpi/util/string.hpp"

inline constexpr auto PERIOD = 20_ms;

namespace {
class OpModeRobotTest {
 public:
  OpModeRobotTest() {
    wpi::sim::PauseTiming();
    wpi::sim::SetProgramStarted(false);
    wpi::sim::DriverStationSim::ResetData();
    wpi::RobotState::ClearOpModes();
  }

  ~OpModeRobotTest() {
    wpi::sim::ResumeTiming();
    wpi::nt::ResetInstance(wpi::nt::GetDefaultInstance());
  }
};

class MockRobot;

class MockOpMode : public wpi::OpMode {
 public:
  std::atomic<uint32_t> m_disabledPeriodicCount{0};
  std::atomic<uint32_t> m_startCount{0};
  std::atomic<uint32_t> m_periodicCount{0};
  std::atomic<uint32_t> m_endCount{0};
  std::atomic<uint32_t> m_closeCount{0};

  MockOpMode() = default;
  ~MockOpMode() override { m_closeCount++; }
  void DisabledPeriodic() override { m_disabledPeriodicCount++; }
  void Start() override { m_startCount++; }
  void Periodic() override { m_periodicCount++; }
  void End() override { m_endCount++; }
};

class OneArgOpMode : public wpi::OpMode {
 public:
  explicit OneArgOpMode(MockRobot& robot) {}
  void Start() override {}
  void End() override {}
};

class MockRobot : public wpi::OpModeRobot<MockRobot> {
 public:
  std::atomic<uint32_t> m_driverStationConnectedCount{0};
  std::atomic<uint32_t> m_nonePeriodicCount{0};

  // RobotPeriodic method counter
  std::atomic<uint32_t> m_robotPeriodicCount{0};

  // Disabled method counters
  std::atomic<uint32_t> m_disabledInitCount{0};
  std::atomic<uint32_t> m_disabledPeriodicCount{0};
  std::atomic<uint32_t> m_disabledExitCount{0};

  MockRobot() = default;

  void DriverStationConnected() override { m_driverStationConnectedCount++; }

  void NonePeriodic() override { m_nonePeriodicCount++; }

  void RobotPeriodic() override { m_robotPeriodicCount++; }

  void DisabledInit() override { m_disabledInitCount++; }

  void DisabledPeriodic() override { m_disabledPeriodicCount++; }

  void DisabledExit() override { m_disabledExitCount++; }
};

// Robot with a single teleop MockOpMode that exposes the most recently created
// instance. The pointer is only valid until the opmode is destroyed.
class SingleOpModeRobot : public MockRobot {
 public:
  std::atomic<MockOpMode*> m_opMode{nullptr};

  SingleOpModeRobot() {
    AddOpModeFactory(wpi::RobotMode::TELEOPERATED, "MockOpMode", [this] {
      auto opMode = std::make_unique<MockOpMode>();
      m_opMode = opMode.get();
      return opMode;
    });
    PublishOpModes();
  }
};

int64_t GetOnlyOpModeId() {
  auto options = wpi::sim::DriverStationSim::GetOpModeOptions();
  REQUIRE(options.size() == 1u);
  return options[0].id;
}
}  // namespace

static_assert(wpi::ConstructibleOpMode<MockOpMode, MockRobot>);
static_assert(wpi::ConstructibleOpMode<OneArgOpMode, MockRobot>);

TEST_CASE_METHOD(OpModeRobotTest, "OpModeRobotTest AddOpMode", "[wpilibc]") {
  struct MyMockRobot : public MockRobot {
    MyMockRobot() {
      AddOpMode<MockOpMode>(wpi::RobotMode::AUTONOMOUS, "NoArgOpMode-Auto",
                            "Group", "Description", wpi::util::Color::WHITE,
                            wpi::util::Color::BLACK);
      AddOpMode<OneArgOpMode>(wpi::RobotMode::UTILITY, "OneArgOpMode-Test",
                              "Group", "Description", wpi::util::Color::WHITE,
                              wpi::util::Color::BLACK);
      AddOpMode<MockOpMode>(wpi::RobotMode::TELEOPERATED, "NoArgOpMode");
      AddOpMode<OneArgOpMode>(wpi::RobotMode::TELEOPERATED, "OneArgOpMode");
      PublishOpModes();
    }
  };
  MyMockRobot robot;

  auto options = wpi::sim::DriverStationSim::GetOpModeOptions();
  REQUIRE(options.size() == 4u);
  int indexes[4] = {-1, -1, -1, -1};
  for (size_t i = 0; i < options.size(); ++i) {
    auto name = wpi::util::to_string_view(&options[i].name);
    if (name == "NoArgOpMode-Auto") {
      indexes[0] = i;
    } else if (name == "OneArgOpMode-Test") {
      indexes[1] = i;
    } else if (name == "NoArgOpMode") {
      indexes[2] = i;
    } else if (name == "OneArgOpMode") {
      indexes[3] = i;
    }
  }

  int i = indexes[0];
  REQUIRE(i != -1);
  CHECK(wpi::util::to_string_view(&options[i].group) == "Group");
  CHECK(wpi::util::to_string_view(&options[i].description) == "Description");
  CHECK(options[i].textColor == 0xffffff);
  CHECK(options[i].backgroundColor == 0x000000);

  i = indexes[1];
  REQUIRE(i != -1);
  CHECK(wpi::util::to_string_view(&options[i].group) == "Group");
  CHECK(wpi::util::to_string_view(&options[i].description) == "Description");
  CHECK(options[i].textColor == 0xffffff);
  CHECK(options[i].backgroundColor == 0x000000);

  i = indexes[2];
  REQUIRE(i != -1);
  CHECK(wpi::util::to_string_view(&options[i].group) == "");
  CHECK(wpi::util::to_string_view(&options[i].description) == "");
  CHECK(options[i].textColor == -1);
  CHECK(options[i].backgroundColor == -1);

  i = indexes[3];
  REQUIRE(i != -1);
  CHECK(wpi::util::to_string_view(&options[i].group) == "");
  CHECK(wpi::util::to_string_view(&options[i].description) == "");
  CHECK(options[i].textColor == -1);
  CHECK(options[i].backgroundColor == -1);
}

TEST_CASE_METHOD(OpModeRobotTest, "OpModeRobotTest ClearOpModes", "[wpilibc]") {
  struct MyMockRobot : public MockRobot {
    MyMockRobot() {
      AddOpMode<MockOpMode>(wpi::RobotMode::TELEOPERATED, "NoArgOpMode");
      AddOpMode<OneArgOpMode>(wpi::RobotMode::TELEOPERATED, "OneArgOpMode");
      PublishOpModes();
    }
  };
  MyMockRobot robot;

  robot.ClearOpModes();
  auto options = wpi::sim::DriverStationSim::GetOpModeOptions();
  CHECK(options.empty());
}

TEST_CASE_METHOD(OpModeRobotTest, "OpModeRobotTest RemoveOpMode", "[wpilibc]") {
  struct MyMockRobot : public MockRobot {
    MyMockRobot() {
      AddOpMode<MockOpMode>(wpi::RobotMode::TELEOPERATED, "NoArgOpMode");
      AddOpMode<OneArgOpMode>(wpi::RobotMode::TELEOPERATED, "OneArgOpMode");
      PublishOpModes();
    }
  };
  MyMockRobot robot;

  robot.RemoveOpMode(wpi::RobotMode::TELEOPERATED, "NoArgOpMode");
  robot.PublishOpModes();
  auto options = wpi::sim::DriverStationSim::GetOpModeOptions();
  REQUIRE(options.size() == 1u);
  CHECK(wpi::util::to_string_view(&options[0].name) == "OneArgOpMode");
}

TEST_CASE_METHOD(OpModeRobotTest, "OpModeRobotTest NonePeriodic", "[wpilibc]") {
  struct MyMockRobot : public MockRobot {
    MyMockRobot() {
      AddOpMode<MockOpMode>(wpi::RobotMode::TELEOPERATED, "NoArgOpMode");
      AddOpMode<OneArgOpMode>(wpi::RobotMode::TELEOPERATED, "OneArgOpMode");
      PublishOpModes();
    }
  };
  MyMockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  // Time step to get periodic calls on 20 ms robot loop
  wpi::sim::StepTiming(110_ms);
  CHECK(robot.m_nonePeriodicCount.load() == 5u);

  robot.EndCompetition();
  robotThread.join();
}

TEST_CASE_METHOD(OpModeRobotTest, "OpModeRobotTest RobotPeriodic",
                 "[wpilibc]") {
  struct MyMockRobot : public MockRobot {
    MyMockRobot() {
      AddOpMode<MockOpMode>(wpi::RobotMode::TELEOPERATED, "TestOpMode");
      PublishOpModes();
    }
  };
  MyMockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  // RobotPeriodic should be called regardless of state
  CHECK(robot.m_robotPeriodicCount.load() == 0u);

  // Step timing to allow callbacks to execute
  wpi::sim::StepTiming(PERIOD);
  CHECK(robot.m_robotPeriodicCount.load() == 1u);

  // Additional time steps should continue calling RobotPeriodic
  wpi::sim::StepTiming(PERIOD);
  CHECK(robot.m_robotPeriodicCount.load() == 2u);

  robot.EndCompetition();
  robotThread.join();
}

TEST_CASE_METHOD(OpModeRobotTest, "OpModeRobotTest DisabledOnStartup",
                 "[wpilibc]") {
  MockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();

  CHECK(robot.m_disabledInitCount.load() == 0u);
  CHECK(robot.m_disabledPeriodicCount.load() == 0u);

  // DisabledInit and DisabledPeriodic both run on the first loop
  wpi::sim::StepTiming(PERIOD);
  CHECK(robot.m_disabledInitCount.load() == 1u);
  CHECK(robot.m_disabledPeriodicCount.load() == 1u);
  CHECK(robot.m_disabledExitCount.load() == 0u);

  wpi::sim::StepTiming(PERIOD);
  CHECK(robot.m_disabledInitCount.load() == 1u);
  CHECK(robot.m_disabledPeriodicCount.load() == 2u);
  CHECK(robot.m_disabledExitCount.load() == 0u);

  robot.EndCompetition();
  robotThread.join();
}

TEST_CASE_METHOD(OpModeRobotTest, "OpModeRobotTest OpModeLifecycle",
                 "[wpilibc]") {
  SingleOpModeRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetDsAttached(true);
  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::SetRobotMode(wpi::RobotMode::TELEOPERATED);
  wpi::sim::DriverStationSim::SetOpMode(GetOnlyOpModeId());
  wpi::sim::DriverStationSim::NotifyNewData();

  // First loop: opmode is created and both disabled periodics run once
  wpi::sim::StepTiming(PERIOD);
  MockOpMode* opMode = robot.m_opMode.load();
  REQUIRE(opMode != nullptr);
  CHECK(robot.m_disabledInitCount.load() == 1u);
  CHECK(robot.m_disabledPeriodicCount.load() == 1u);
  CHECK(opMode->m_disabledPeriodicCount.load() == 1u);
  CHECK(opMode->m_startCount.load() == 0u);

  wpi::sim::StepTiming(PERIOD);
  CHECK(robot.m_disabledInitCount.load() == 1u);
  CHECK(robot.m_disabledPeriodicCount.load() == 2u);
  CHECK(opMode->m_disabledPeriodicCount.load() == 2u);

  // Enable: DisabledExit runs and the opmode starts
  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(PERIOD);
  CHECK(robot.m_disabledExitCount.load() == 1u);
  CHECK(robot.m_disabledPeriodicCount.load() == 2u);
  CHECK(opMode->m_disabledPeriodicCount.load() == 2u);
  CHECK(opMode->m_startCount.load() == 1u);
  CHECK(opMode->m_endCount.load() == 0u);

  // Disable: the opmode ends and DisabledInit runs again
  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(PERIOD);
  CHECK(robot.m_disabledInitCount.load() == 2u);
  CHECK(robot.m_disabledPeriodicCount.load() == 3u);
  CHECK(robot.m_disabledExitCount.load() == 1u);

  robot.EndCompetition();
  robotThread.join();
}

TEST_CASE_METHOD(OpModeRobotTest, "OpModeRobotTest OpModeCreatedWhileEnabled",
                 "[wpilibc]") {
  SingleOpModeRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetDsAttached(true);
  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::SetRobotMode(wpi::RobotMode::TELEOPERATED);
  wpi::sim::DriverStationSim::SetOpMode(GetOnlyOpModeId());
  wpi::sim::DriverStationSim::NotifyNewData();

  // Opmode gets one DisabledPeriodic call, then starts on the same loop; the
  // robot's disabled functions do not run
  wpi::sim::StepTiming(PERIOD);
  MockOpMode* opMode = robot.m_opMode.load();
  REQUIRE(opMode != nullptr);
  CHECK(opMode->m_disabledPeriodicCount.load() == 1u);
  CHECK(opMode->m_startCount.load() == 1u);
  CHECK(robot.m_disabledInitCount.load() == 0u);
  CHECK(robot.m_disabledPeriodicCount.load() == 0u);
  CHECK(robot.m_disabledExitCount.load() == 0u);

  robot.EndCompetition();
  robotThread.join();
}
