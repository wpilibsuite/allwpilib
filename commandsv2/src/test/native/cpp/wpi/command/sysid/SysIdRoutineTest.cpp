// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/commands2/Subsystem.hpp"
#include "wpi/commands2/sysid/SysIdRoutine.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/system/Timer.hpp"
#include "wpi/units/math.hpp"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(wpi::units::math::abs(val1 - val2), eps)

enum StateTest {
  Invalid,
  InRecordStateQf,
  InRecordStateQr,
  InRecordStateDf,
  InRecordStateDr,
  InDrive,
  InLog,
  DoneWithRecordState
};

class SysIdRoutineTest : public ::testing::Test {
 protected:
  std::vector<StateTest> currentStateList{};
  std::vector<wpi::units::volt_t> sentVoltages{};
  wpi::cmd::Subsystem m_subsystem{};
  wpi::cmd::sysid::SysIdRoutine m_sysidRoutine{
      wpi::cmd::sysid::Config{
          std::nullopt, std::nullopt, std::nullopt,
          [this](wpi::sysid::State state) {
            switch (state) {
              case wpi::sysid::State::kQuasistaticForward:
                currentStateList.emplace_back(StateTest::InRecordStateQf);
                break;
              case wpi::sysid::State::kQuasistaticReverse:
                currentStateList.emplace_back(StateTest::InRecordStateQr);
                break;
              case wpi::sysid::State::kDynamicForward:
                currentStateList.emplace_back(StateTest::InRecordStateDf);
                break;
              case wpi::sysid::State::kDynamicReverse:
                currentStateList.emplace_back(StateTest::InRecordStateDr);
                break;
              case wpi::sysid::State::kNone:
                currentStateList.emplace_back(StateTest::DoneWithRecordState);
                break;
            }
          }},
      wpi::cmd::sysid::Mechanism{
          [this](wpi::units::volt_t driveVoltage) {
            sentVoltages.emplace_back(driveVoltage);
            currentStateList.emplace_back(StateTest::InDrive);
          },
          [this](wpi::sysid::SysIdRoutineLog* log) {
            currentStateList.emplace_back(StateTest::InLog);
            log->Motor("Mock Motor").position(0_m).velocity(0_mps).voltage(0_V);
          },
          &m_subsystem}};

  wpi::cmd::CommandPtr m_quasistaticForward{
      m_sysidRoutine.Quasistatic(wpi::cmd::sysid::Direction::kForward)};
  wpi::cmd::CommandPtr m_quasistaticReverse{
      m_sysidRoutine.Quasistatic(wpi::cmd::sysid::Direction::kReverse)};
  wpi::cmd::CommandPtr m_dynamicForward{
      m_sysidRoutine.Dynamic(wpi::cmd::sysid::Direction::kForward)};
  wpi::cmd::CommandPtr m_dynamicReverse{
      m_sysidRoutine.Dynamic(wpi::cmd::sysid::Direction::kReverse)};

  wpi::cmd::sysid::SysIdRoutine m_emptySysidRoutine{
      wpi::cmd::sysid::Config{std::nullopt, std::nullopt, std::nullopt,
                              nullptr},
      wpi::cmd::sysid::Mechanism{[](wpi::units::volt_t driveVoltage) {},
                                 nullptr, &m_subsystem}};

  wpi::cmd::CommandPtr m_emptyRoutineForward{
      m_emptySysidRoutine.Quasistatic(wpi::cmd::sysid::Direction::kForward)};

  void RunCommand(wpi::cmd::CommandPtr command) {
    command.get()->Initialize();
    command.get()->Execute();
    wpi::sim::StepTiming(1_s);
    command.get()->Execute();
    command.get()->End(true);
  }

  void SetUp() override {
    wpi::sim::PauseTiming();
    wpi::cmd::CommandPtr m_quasistaticForward{
        m_sysidRoutine.Quasistatic(wpi::cmd::sysid::Direction::kForward)};
    wpi::cmd::CommandPtr m_quasistaticReverse{
        m_sysidRoutine.Quasistatic(wpi::cmd::sysid::Direction::kReverse)};
    wpi::cmd::CommandPtr m_dynamicForward{
        m_sysidRoutine.Dynamic(wpi::cmd::sysid::Direction::kForward)};
    wpi::cmd::CommandPtr m_dynamicReverse{
        m_sysidRoutine.Dynamic(wpi::cmd::sysid::Direction::kReverse)};
  }

  void TearDown() override { wpi::sim::ResumeTiming(); }
};

TEST_F(SysIdRoutineTest, RecordStateBookendsMotorLogging) {
  RunCommand(std::move(m_quasistaticForward));
  std::vector<StateTest> expectedOrder{
      StateTest::InDrive, StateTest::InLog, StateTest::InRecordStateQf,
      StateTest::InDrive, StateTest::DoneWithRecordState};
  EXPECT_TRUE(expectedOrder == currentStateList);
  currentStateList.clear();
  sentVoltages.clear();

  expectedOrder = std::vector<StateTest>{
      StateTest::InDrive, StateTest::InLog, StateTest::InRecordStateDf,
      StateTest::InDrive, StateTest::DoneWithRecordState};
  RunCommand(std::move(m_dynamicForward));
  EXPECT_TRUE(expectedOrder == currentStateList);
  currentStateList.clear();
  sentVoltages.clear();
}

TEST_F(SysIdRoutineTest, DeclareCorrectState) {
  RunCommand(std::move(m_quasistaticForward));
  EXPECT_TRUE(std::find(currentStateList.begin(), currentStateList.end(),
                        StateTest::InRecordStateQf) != currentStateList.end());
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_quasistaticReverse));
  EXPECT_TRUE(std::find(currentStateList.begin(), currentStateList.end(),
                        StateTest::InRecordStateQr) != currentStateList.end());
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_dynamicForward));
  EXPECT_TRUE(std::find(currentStateList.begin(), currentStateList.end(),
                        StateTest::InRecordStateDf) != currentStateList.end());
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_dynamicReverse));
  EXPECT_TRUE(std::find(currentStateList.begin(), currentStateList.end(),
                        StateTest::InRecordStateDr) != currentStateList.end());
  currentStateList.clear();
  sentVoltages.clear();
}

TEST_F(SysIdRoutineTest, OutputCorrectVoltage) {
  RunCommand(std::move(m_quasistaticForward));
  std::vector<wpi::units::volt_t> expectedVoltages{1_V, 0_V};
  EXPECT_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  EXPECT_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_quasistaticReverse));
  expectedVoltages = std::vector<wpi::units::volt_t>{-1_V, 0_V};
  EXPECT_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  EXPECT_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_dynamicForward));
  expectedVoltages = std::vector<wpi::units::volt_t>{7_V, 0_V};
  EXPECT_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  EXPECT_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_dynamicReverse));
  expectedVoltages = std::vector<wpi::units::volt_t>{-7_V, 0_V};
  EXPECT_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  EXPECT_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();
}

TEST_F(SysIdRoutineTest, EmptyLogFunc) {
  RunCommand(std::move(m_emptyRoutineForward));
  SUCCEED();
}
