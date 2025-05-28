// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc2/command/Subsystem.h>
#include <frc2/command/sysid/SysIdRoutine.h>

#include <utility>
#include <vector>

#include <frc/Timer.h>
#include <frc/simulation/SimHooks.h>
#include <gtest/gtest.h>
#include <units/math.h>

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

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
  std::vector<units::volt_t> sentVoltages{};
  frc2::Subsystem m_subsystem{};
  frc2::sysid::SysIdRoutine m_sysidRoutine{
      frc2::sysid::Config{
          std::nullopt, std::nullopt, std::nullopt,
          [this](frc::sysid::State state) {
            switch (state) {
              case frc::sysid::State::kSweepForward:
                currentStateList.emplace_back(StateTest::InRecordStateQf);
                break;
              case frc::sysid::State::kSweepReverse:
                currentStateList.emplace_back(StateTest::InRecordStateQr);
                break;
              case frc::sysid::State::kStepForward:
                currentStateList.emplace_back(StateTest::InRecordStateDf);
                break;
              case frc::sysid::State::kStepReverse:
                currentStateList.emplace_back(StateTest::InRecordStateDr);
                break;
              case frc::sysid::State::kNone:
                currentStateList.emplace_back(StateTest::DoneWithRecordState);
                break;
            }
          }},
      frc2::sysid::Mechanism{
          [this](units::volt_t driveVoltage) {
            sentVoltages.emplace_back(driveVoltage);
            currentStateList.emplace_back(StateTest::InDrive);
          },
          [this](frc::sysid::SysIdRoutineLog* log) {
            currentStateList.emplace_back(StateTest::InLog);
            log->Motor("Mock Motor").position(0_m).velocity(0_mps).voltage(0_V);
          },
          &m_subsystem}};

  frc2::CommandPtr m_sweepForward{
      m_sysidRoutine.Sweep(frc2::sysid::Direction::kForward)};
  frc2::CommandPtr m_sweepReverse{
      m_sysidRoutine.Sweep(frc2::sysid::Direction::kReverse)};
  frc2::CommandPtr m_stepForward{
      m_sysidRoutine.Step(frc2::sysid::Direction::kForward)};
  frc2::CommandPtr m_stepReverse{
      m_sysidRoutine.Step(frc2::sysid::Direction::kReverse)};

  frc2::sysid::SysIdRoutine m_emptySysidRoutine{
      frc2::sysid::Config{std::nullopt, std::nullopt, std::nullopt, nullptr},
      frc2::sysid::Mechanism{[](units::volt_t driveVoltage) {}, nullptr,
                             &m_subsystem}};

  frc2::CommandPtr m_emptyRoutineForward{
      m_emptySysidRoutine.Sweep(frc2::sysid::Direction::kForward)};

  void RunCommand(frc2::CommandPtr command) {
    command.get()->Initialize();
    command.get()->Execute();
    frc::sim::StepTiming(1_s);
    command.get()->Execute();
    command.get()->End(true);
  }

  void SetUp() override {
    frc::sim::PauseTiming();
    frc2::CommandPtr m_sweepForward{
        m_sysidRoutine.Sweep(frc2::sysid::Direction::kForward)};
    frc2::CommandPtr m_sweepReverse{
        m_sysidRoutine.Sweep(frc2::sysid::Direction::kReverse)};
    frc2::CommandPtr m_stepForward{
        m_sysidRoutine.Step(frc2::sysid::Direction::kForward)};
    frc2::CommandPtr m_stepReverse{
        m_sysidRoutine.Step(frc2::sysid::Direction::kReverse)};
  }

  void TearDown() override { frc::sim::ResumeTiming(); }
};

TEST_F(SysIdRoutineTest, RecordStateBookendsMotorLogging) {
  RunCommand(std::move(m_sweepForward));
  std::vector<StateTest> expectedOrder{
      StateTest::InDrive, StateTest::InLog, StateTest::InRecordStateQf,
      StateTest::InDrive, StateTest::DoneWithRecordState};
  EXPECT_TRUE(expectedOrder == currentStateList);
  currentStateList.clear();
  sentVoltages.clear();

  expectedOrder = std::vector<StateTest>{
      StateTest::InDrive, StateTest::InLog, StateTest::InRecordStateDf,
      StateTest::InDrive, StateTest::DoneWithRecordState};
  RunCommand(std::move(m_stepForward));
  EXPECT_TRUE(expectedOrder == currentStateList);
  currentStateList.clear();
  sentVoltages.clear();
}

TEST_F(SysIdRoutineTest, DeclareCorrectState) {
  RunCommand(std::move(m_sweepForward));
  EXPECT_TRUE(std::find(currentStateList.begin(), currentStateList.end(),
                        StateTest::InRecordStateQf) != currentStateList.end());
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_sweepReverse));
  EXPECT_TRUE(std::find(currentStateList.begin(), currentStateList.end(),
                        StateTest::InRecordStateQr) != currentStateList.end());
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_stepForward));
  EXPECT_TRUE(std::find(currentStateList.begin(), currentStateList.end(),
                        StateTest::InRecordStateDf) != currentStateList.end());
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_stepReverse));
  EXPECT_TRUE(std::find(currentStateList.begin(), currentStateList.end(),
                        StateTest::InRecordStateDr) != currentStateList.end());
  currentStateList.clear();
  sentVoltages.clear();
}

TEST_F(SysIdRoutineTest, OutputCorrectVoltage) {
  RunCommand(std::move(m_sweepForward));
  std::vector<units::volt_t> expectedVoltages{1_V, 0_V};
  EXPECT_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  EXPECT_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_sweepReverse));
  expectedVoltages = std::vector<units::volt_t>{-1_V, 0_V};
  EXPECT_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  EXPECT_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_stepForward));
  expectedVoltages = std::vector<units::volt_t>{7_V, 0_V};
  EXPECT_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  EXPECT_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_stepReverse));
  expectedVoltages = std::vector<units::volt_t>{-7_V, 0_V};
  EXPECT_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  EXPECT_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();
}

TEST_F(SysIdRoutineTest, EmptyLogFunc) {
  RunCommand(std::move(m_emptyRoutineForward));
  SUCCEED();
}
