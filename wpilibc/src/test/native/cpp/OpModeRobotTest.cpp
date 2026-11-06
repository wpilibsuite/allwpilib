// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/OpModeRobot.hpp"

#include <sys/types.h>

#include <format>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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

// Shared event log written by the robot thread and read by the test thread.
class EventLog {
 public:
  void Add(std::string event) {
    std::scoped_lock lock{m_mutex};
    m_events.push_back(std::move(event));
  }

  std::vector<std::string> Get() {
    std::scoped_lock lock{m_mutex};
    return m_events;
  }

  void Clear() {
    std::scoped_lock lock{m_mutex};
    m_events.clear();
  }

 private:
  std::mutex m_mutex;
  std::vector<std::string> m_events;
};

// Listener that appends "<tag>:<event>:<opModeName>" to a shared log.
class LoggingListener : public wpi::OpModeListener {
 public:
  LoggingListener(std::string tag, EventLog& log)
      : m_tag{std::move(tag)}, m_log{log} {}

  void OnOpModeSelect(std::string_view name, wpi::OpMode&) override {
    Log("select", name);
  }
  void OnOpModeStart(std::string_view name, wpi::OpMode&) override {
    Log("start", name);
  }
  void OnOpModeStop(std::string_view name, wpi::OpMode&) override {
    Log("stop", name);
  }
  void OnOpModeClose(std::string_view name, wpi::OpMode&) override {
    Log("close", name);
  }

 private:
  void Log(std::string_view event, std::string_view name) {
    m_log.Add(std::format("{}:{}:{}", m_tag, event, name));
  }

  std::string m_tag;
  EventLog& m_log;
};

void SelectTeleop(bool enabled) {
  wpi::sim::DriverStationSim::SetDsAttached(true);
  wpi::sim::DriverStationSim::SetRobotMode(wpi::RobotMode::TELEOPERATED);
  wpi::sim::DriverStationSim::SetOpMode(GetOnlyOpModeId());
  wpi::sim::DriverStationSim::SetEnabled(enabled);
  wpi::sim::DriverStationSim::NotifyNewData();
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

TEST_CASE_METHOD(OpModeRobotTest, "OpModeRobotTest ListenerLifecycleEvents",
                 "[wpilibc]") {
  using Events = std::vector<std::string>;
  EventLog log;
  SingleOpModeRobot robot;
  robot.RegisterListener(std::make_shared<LoggingListener>("A", log));

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  // Selected while disabled: only select fires
  SelectTeleop(false);
  wpi::sim::StepTiming(PERIOD);
  CHECK(log.Get() == Events{"A:select:MockOpMode"});

  // Enabled: start fires
  SelectTeleop(true);
  wpi::sim::StepTiming(PERIOD);
  CHECK(log.Get() == Events{"A:select:MockOpMode", "A:start:MockOpMode"});

  // Disabled: stop then close fire
  log.Clear();
  SelectTeleop(false);
  wpi::sim::StepTiming(PERIOD);
  auto events = log.Get();
  REQUIRE(events.size() >= 2u);
  CHECK(events[0] == "A:stop:MockOpMode");
  CHECK(events[1] == "A:close:MockOpMode");

  robot.EndCompetition();
  robotThread.join();
}

TEST_CASE_METHOD(OpModeRobotTest, "OpModeRobotTest ListenerOrdering",
                 "[wpilibc]") {
  using Events = std::vector<std::string>;
  EventLog log;
  SingleOpModeRobot robot;
  robot.RegisterListener(std::make_shared<LoggingListener>("A", log));
  robot.RegisterListener(std::make_shared<LoggingListener>("B", log));

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  SelectTeleop(true);
  wpi::sim::StepTiming(PERIOD);
  // Select and start run in registration order
  CHECK(log.Get() == Events{"A:select:MockOpMode", "B:select:MockOpMode",
                            "A:start:MockOpMode", "B:start:MockOpMode"});

  log.Clear();
  SelectTeleop(false);
  wpi::sim::StepTiming(PERIOD);
  // Stop and close run in reverse registration order
  auto events = log.Get();
  REQUIRE(events.size() >= 4u);
  CHECK(Events(events.begin(), events.begin() + 4) ==
        Events{"B:stop:MockOpMode", "A:stop:MockOpMode", "B:close:MockOpMode",
               "A:close:MockOpMode"});

  robot.EndCompetition();
  robotThread.join();
}

TEST_CASE_METHOD(OpModeRobotTest, "OpModeRobotTest RemoveListener",
                 "[wpilibc]") {
  using Events = std::vector<std::string>;
  EventLog log;
  SingleOpModeRobot robot;
  auto removed = std::make_shared<LoggingListener>("removed", log);
  robot.RegisterListener(removed);
  robot.RegisterListener(std::make_shared<LoggingListener>("kept", log));
  robot.RemoveListener(removed);

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  SelectTeleop(true);
  wpi::sim::StepTiming(PERIOD);
  CHECK(log.Get() == Events{"kept:select:MockOpMode", "kept:start:MockOpMode"});

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

  // No further DisabledPeriodic calls while enabled
  wpi::sim::StepTiming(PERIOD);
  CHECK(opMode->m_disabledPeriodicCount.load() == 1u);

  robot.EndCompetition();
  robotThread.join();
}

TEST_CASE_METHOD(OpModeRobotTest,
                 "OpModeRobotTest TeleopCreatedWhileEnabledAfterAutoWithFms",
                 "[wpilibc]") {
  // Under FMS the teleop opmode is selected while the robot stays enabled
  // after auto ends
  struct AutoTeleopRobot : public MockRobot {
    std::atomic<MockOpMode*> m_auto{nullptr};
    std::atomic<MockOpMode*> m_teleop{nullptr};

    AutoTeleopRobot() {
      AddOpModeFactory(wpi::RobotMode::AUTONOMOUS, "Auto", [this] {
        auto opMode = std::make_unique<MockOpMode>();
        m_auto = opMode.get();
        return opMode;
      });
      AddOpModeFactory(wpi::RobotMode::TELEOPERATED, "Teleop", [this] {
        auto opMode = std::make_unique<MockOpMode>();
        m_teleop = opMode.get();
        return opMode;
      });
      PublishOpModes();
    }
  };
  AutoTeleopRobot robot;

  int64_t autoId = -1;
  int64_t teleopId = -1;
  for (auto& option : wpi::sim::DriverStationSim::GetOpModeOptions()) {
    auto name = wpi::util::to_string_view(&option.name);
    if (name == "Auto") {
      autoId = option.id;
    } else if (name == "Teleop") {
      teleopId = option.id;
    }
  }
  REQUIRE(autoId != -1);
  REQUIRE(teleopId != -1);

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetDsAttached(true);
  wpi::sim::DriverStationSim::SetFmsAttached(true);
  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::SetRobotMode(wpi::RobotMode::AUTONOMOUS);
  wpi::sim::DriverStationSim::SetOpMode(autoId);
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(PERIOD);

  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(PERIOD);
  MockOpMode* autoOpMode = robot.m_auto.load();
  REQUIRE(autoOpMode != nullptr);
  CHECK(autoOpMode->m_startCount.load() == 1u);
  CHECK(robot.m_teleop.load() == nullptr);

  // Auto ends and teleop is selected without the robot ever being disabled
  wpi::sim::DriverStationSim::SetRobotMode(wpi::RobotMode::TELEOPERATED);
  wpi::sim::DriverStationSim::SetOpMode(teleopId);
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(PERIOD);
  MockOpMode* teleopOpMode = robot.m_teleop.load();
  REQUIRE(teleopOpMode != nullptr);
  CHECK(teleopOpMode->m_disabledPeriodicCount.load() == 1u);
  CHECK(teleopOpMode->m_startCount.load() == 1u);
  // Only the single disabled loop before the first enable ran the robot's
  // DisabledPeriodic
  CHECK(robot.m_disabledPeriodicCount.load() == 1u);

  robot.EndCompetition();
  robotThread.join();
}
