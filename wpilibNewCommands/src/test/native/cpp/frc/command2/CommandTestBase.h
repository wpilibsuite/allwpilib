// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <utility>

#include <frc/simulation/DriverStationSim.h>
#include <gtest/gtest.h>

#include "frc/command2/CommandHelper.h"
#include "frc/command2/CommandScheduler.h"
#include "frc/command2/SubsystemBase.h"
#include "gmock/gmock.h"
#include "make_vector.h"

class TestSubsystem : public frc::SubsystemBase {
 public:
  explicit TestSubsystem(std::function<void()> periodic = [] {})
      : m_periodic{periodic} {}
  void Periodic() override { m_periodic(); }

 private:
  std::function<void()> m_periodic;
};

/**
 * NOTE: Moving mock objects causes EXPECT_CALL to not work correctly!
 */
class MockCommand : public frc::CommandHelper<frc::Command, MockCommand> {
 public:
  MOCK_CONST_METHOD0(GetRequirements, wpi::SmallSet<frc::Subsystem*, 4>());
  MOCK_METHOD0(IsFinished, bool());
  MOCK_CONST_METHOD0(RunsWhenDisabled, bool());
  MOCK_METHOD0(Initialize, void());
  MOCK_METHOD0(Execute, void());
  MOCK_METHOD1(End, void(bool interrupted));

  MockCommand() {
    m_requirements = {};
    EXPECT_CALL(*this, GetRequirements())
        .WillRepeatedly(testing::Return(m_requirements));
    EXPECT_CALL(*this, IsFinished()).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(*this, RunsWhenDisabled())
        .WillRepeatedly(testing::Return(true));
  }

  MockCommand(std::initializer_list<frc::Subsystem*> requirements,
              bool finished = false, bool runWhenDisabled = true) {
    m_requirements.insert(requirements.begin(), requirements.end());
    EXPECT_CALL(*this, GetRequirements())
        .WillRepeatedly(testing::Return(m_requirements));
    EXPECT_CALL(*this, IsFinished()).WillRepeatedly(testing::Return(finished));
    EXPECT_CALL(*this, RunsWhenDisabled())
        .WillRepeatedly(testing::Return(runWhenDisabled));
  }

  MockCommand(MockCommand&& other) {
    EXPECT_CALL(*this, IsFinished())
        .WillRepeatedly(testing::Return(other.IsFinished()));
    EXPECT_CALL(*this, RunsWhenDisabled())
        .WillRepeatedly(testing::Return(other.RunsWhenDisabled()));
    std::swap(m_requirements, other.m_requirements);
    EXPECT_CALL(*this, GetRequirements())
        .WillRepeatedly(testing::Return(m_requirements));
  }

  MockCommand(const MockCommand& other) : CommandHelper{other} {}

  void SetFinished(bool finished) {
    EXPECT_CALL(*this, IsFinished()).WillRepeatedly(testing::Return(finished));
  }

  ~MockCommand() {  // NOLINT
    auto& scheduler = frc::CommandScheduler::GetInstance();
    scheduler.Cancel(this);
  }

 private:
  wpi::SmallSet<frc::Subsystem*, 4> m_requirements;
};

class CommandTestBase : public testing::Test {
 public:
  CommandTestBase();

  ~CommandTestBase() override;

 protected:
  frc::CommandScheduler GetScheduler();

  void SetDSEnabled(bool enabled);
};

template <typename T>
class CommandTestBaseWithParam : public testing::TestWithParam<T> {
 public:
  CommandTestBaseWithParam() {
    auto& scheduler = frc::CommandScheduler::GetInstance();
    scheduler.CancelAll();
    scheduler.Enable();
    scheduler.GetActiveButtonLoop()->Clear();
    scheduler.UnregisterAllSubsystems();

    SetDSEnabled(true);
  }

  ~CommandTestBaseWithParam() override {
    frc::CommandScheduler::GetInstance().GetActiveButtonLoop()->Clear();
    frc::CommandScheduler::GetInstance().UnregisterAllSubsystems();
  }

 protected:
  frc::CommandScheduler GetScheduler() { return frc::CommandScheduler(); }

  void SetDSEnabled(bool enabled) {
    frc::sim::DriverStationSim::SetDsAttached(true);
    frc::sim::DriverStationSim::SetEnabled(enabled);
    frc::sim::DriverStationSim::NotifyNewData();
  }
};
