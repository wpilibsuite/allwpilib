// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <optional>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "wpi/commands2/CommandHelper.hpp"
#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/Requirements.hpp"
#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/simulation/DriverStationSim.hpp"

namespace wpi::cmd {

class TestSubsystem : public SubsystemBase {
 public:
  explicit TestSubsystem(std::function<void()> periodic = [] {})
      : m_periodic{std::move(periodic)} {}
  void Periodic() override { m_periodic(); }

 private:
  std::function<void()> m_periodic;
};

class MockCommand : public CommandHelper<Command, MockCommand> {
 public:
  MockCommand() = default;

  explicit MockCommand(Requirements requirements, bool finished = false,
                       bool runWhenDisabled = true)
      : m_finished{finished}, m_runsWhenDisabled{runWhenDisabled} {
    m_requirements.insert(requirements.begin(), requirements.end());
  }

  MockCommand(MockCommand&& other) noexcept
      : CommandHelper{std::move(other)},
        m_requirements{std::move(other.m_requirements)},
        m_finished{other.m_finished},
        m_runsWhenDisabled{other.m_runsWhenDisabled} {}

  MockCommand(const MockCommand& other)
      : CommandHelper{other},
        m_requirements{other.m_requirements},
        m_finished{other.m_finished},
        m_runsWhenDisabled{other.m_runsWhenDisabled} {}

  wpi::util::SmallSet<Subsystem*, 4> GetRequirements() const override {
    return m_requirements;
  }

  bool IsFinished() override { return m_finished; }

  bool RunsWhenDisabled() const override { return m_runsWhenDisabled; }

  void Initialize() override { ++m_initializeCount; }

  void Execute() override { ++m_executeCount; }

  void End(bool interrupted) override {
    if (interrupted) {
      ++m_endTrueCount;
    } else {
      ++m_endFalseCount;
    }
  }

  void SetFinished(bool finished) { m_finished = finished; }

  void ExpectInitialize(int count = 1) { m_expectedInitialize = count; }

  void ExpectExecute(int count = 1) { m_expectedExecute = count; }

  void ExpectEnd(bool interrupted, int count = 1) {
    if (interrupted) {
      m_expectedEndTrue = count;
    } else {
      m_expectedEndFalse = count;
    }
  }

  ~MockCommand() {  // NOLINT
    auto& scheduler = CommandScheduler::GetInstance();
    scheduler.Cancel(this);
    Verify();
  }

  void Verify() const {
    if (m_expectedInitialize) {
      CHECK(m_initializeCount == *m_expectedInitialize);
    }
    if (m_expectedExecute) {
      CHECK(m_executeCount == *m_expectedExecute);
    }
    if (m_expectedEndTrue) {
      CHECK(m_endTrueCount == *m_expectedEndTrue);
    }
    if (m_expectedEndFalse) {
      CHECK(m_endFalseCount == *m_expectedEndFalse);
    }
  }

 private:
  wpi::util::SmallSet<Subsystem*, 4> m_requirements;
  bool m_finished = false;
  bool m_runsWhenDisabled = true;
  int m_initializeCount = 0;
  int m_executeCount = 0;
  int m_endTrueCount = 0;
  int m_endFalseCount = 0;
  std::optional<int> m_expectedInitialize;
  std::optional<int> m_expectedExecute;
  std::optional<int> m_expectedEndTrue;
  std::optional<int> m_expectedEndFalse;
};

class CommandTestBase {
 public:
  CommandTestBase();

  virtual ~CommandTestBase();

  CommandScheduler GetScheduler();

  void SetDSEnabled(bool enabled);
};

template <typename T>
class CommandTestBaseWithParam {
 public:
  CommandTestBaseWithParam() {
    auto& scheduler = CommandScheduler::GetInstance();
    scheduler.CancelAll();
    scheduler.Enable();
    scheduler.GetActiveButtonLoop()->Clear();
    scheduler.UnregisterAllSubsystems();

    SetDSEnabled(true);
  }

  virtual ~CommandTestBaseWithParam() {
    CommandScheduler::GetInstance().GetActiveButtonLoop()->Clear();
    CommandScheduler::GetInstance().UnregisterAllSubsystems();
  }

  CommandScheduler GetScheduler() { return CommandScheduler(); }

  void SetDSEnabled(bool enabled) {
    wpi::sim::DriverStationSim::SetDsAttached(true);
    wpi::sim::DriverStationSim::SetEnabled(enabled);
    wpi::sim::DriverStationSim::NotifyNewData();
  }
};

}  // namespace wpi::cmd
