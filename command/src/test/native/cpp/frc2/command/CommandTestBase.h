// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <utility>

#include <frc/simulation/DriverStationSim.h>
#include <gtest/gtest.h>

#include "frc2/command/CommandHelper.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/Requirements.h"
#include "frc2/command/SubsystemBase.h"
#include "gmock/gmock.h"

namespace frc2 {

class TestSubsystem : public SubsystemBase {
 public:
  explicit TestSubsystem(std::function<void()> periodic = [] {})
      : m_periodic{std::move(periodic)} {}
  void Periodic() override { m_periodic(); }

 private:
  std::function<void()> m_periodic;
};

/**
 * NOTE: Moving mock objects causes EXPECT_CALL to not work correctly!
 */
class MockCommand : public CommandHelper<Command, MockCommand> {
 public:
  MOCK_CONST_METHOD0(GetRequirements, wpi::SmallSet<Subsystem*, 4>());
  MOCK_METHOD0(IsFinished, bool());
  MOCK_CONST_METHOD0(RunsWhenDisabled, bool());
  MOCK_METHOD0(Initialize, void());
  MOCK_METHOD0(Execute, void());
  MOCK_METHOD1(End, void(bool interrupted));

  MockCommand() {
    m_requirements = {};
    EXPECT_CALL(*this, GetRequirements())
        .WillRepeatedly(::testing::Return(m_requirements));
    EXPECT_CALL(*this, IsFinished()).WillRepeatedly(::testing::Return(false));
    EXPECT_CALL(*this, RunsWhenDisabled())
        .WillRepeatedly(::testing::Return(true));
  }

  explicit MockCommand(Requirements requirements, bool finished = false,
                       bool runWhenDisabled = true) {
    m_requirements.insert(requirements.begin(), requirements.end());
    EXPECT_CALL(*this, GetRequirements())
        .WillRepeatedly(::testing::Return(m_requirements));
    EXPECT_CALL(*this, IsFinished())
        .WillRepeatedly(::testing::Return(finished));
    EXPECT_CALL(*this, RunsWhenDisabled())
        .WillRepeatedly(::testing::Return(runWhenDisabled));
  }

  MockCommand(MockCommand&& other) {
    EXPECT_CALL(*this, IsFinished())
        .WillRepeatedly(::testing::Return(other.IsFinished()));
    EXPECT_CALL(*this, RunsWhenDisabled())
        .WillRepeatedly(::testing::Return(other.RunsWhenDisabled()));
    std::swap(m_requirements, other.m_requirements);
    EXPECT_CALL(*this, GetRequirements())
        .WillRepeatedly(::testing::Return(m_requirements));
  }

  MockCommand(const MockCommand& other) : CommandHelper{other} {}

  void SetFinished(bool finished) {
    EXPECT_CALL(*this, IsFinished())
        .WillRepeatedly(::testing::Return(finished));
  }

  ~MockCommand() {  // NOLINT
    auto& scheduler = CommandScheduler::GetInstance();
    scheduler.Cancel(this);
  }

 private:
  wpi::SmallSet<Subsystem*, 4> m_requirements;
};

class CommandTestBase : public ::testing::Test {
 public:
  CommandTestBase();

  ~CommandTestBase() override;

 protected:
  CommandScheduler GetScheduler();

  void SetDSEnabled(bool enabled);
};

template <typename T>
class CommandTestBaseWithParam : public ::testing::TestWithParam<T> {
 public:
  CommandTestBaseWithParam() {
    auto& scheduler = CommandScheduler::GetInstance();
    scheduler.CancelAll();
    scheduler.Enable();
    scheduler.GetActiveButtonLoop()->Clear();
    scheduler.UnregisterAllSubsystems();

    SetDSEnabled(true);
  }

  ~CommandTestBaseWithParam() override {
    CommandScheduler::GetInstance().GetActiveButtonLoop()->Clear();
    CommandScheduler::GetInstance().UnregisterAllSubsystems();
  }

 protected:
  CommandScheduler GetScheduler() { return CommandScheduler(); }

  void SetDSEnabled(bool enabled) {
    frc::sim::DriverStationSim::SetDsAttached(true);
    frc::sim::DriverStationSim::SetEnabled(enabled);
    frc::sim::DriverStationSim::NotifyNewData();
  }
};

}  // namespace frc2
