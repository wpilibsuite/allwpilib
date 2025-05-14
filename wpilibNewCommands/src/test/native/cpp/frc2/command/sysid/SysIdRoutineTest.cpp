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
              case frc::sysid::State::QUASISTATIC_FORWARD:
                currentStateList.emplace_back(StateTest::InRecordStateQf);
                break;
              case frc::sysid::State::QUASISTATIC_REVERSE:
                currentStateList.emplace_back(StateTest::InRecordStateQr);
                break;
              case frc::sysid::State::DYNAMIC_FORWARD:
                currentStateList.emplace_back(StateTest::InRecordStateDf);
                break;
              case frc::sysid::State::DYNAMIC_REVERSE:
                currentStateList.emplace_back(StateTest::InRecordStateDr);
                break;
              case frc::sysid::State::NONE:
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

  frc2::CommandPtr m_quasistaticForward{
      m_sysidRoutine.Quasistatic(frc2::sysid::Direction::FORWARD)};
  frc2::CommandPtr m_quasistaticReverse{
      m_sysidRoutine.Quasistatic(frc2::sysid::Direction::REVERSE)};
  frc2::CommandPtr m_dynamicForward{
      m_sysidRoutine.Dynamic(frc2::sysid::Direction::FORWARD)};
  frc2::CommandPtr m_dynamicReverse{
      m_sysidRoutine.Dynamic(frc2::sysid::Direction::REVERSE)};

  frc2::sysid::SysIdRoutine m_emptySysidRoutine{
      frc2::sysid::Config{std::nullopt, std::nullopt, std::nullopt, nullptr},
      frc2::sysid::Mechanism{[](units::volt_t driveVoltage) {}, nullptr,
                             &m_subsystem}};

  frc2::CommandPtr m_emptyRoutineForward{
      m_emptySysidRoutine.Quasistatic(frc2::sysid::Direction::FORWARD)};

  void RunCommand(frc2::CommandPtr command) {
    command.get()->Initialize();
    command.get()->Execute();
    frc::sim::StepTiming(1_s);
    command.get()->Execute();
    command.get()->End(true);
  }

  void SetUp() override {
    frc::sim::PauseTiming();
    frc2::CommandPtr m_quasistaticForward{
        m_sysidRoutine.Quasistatic(frc2::sysid::Direction::FORWARD)};
    frc2::CommandPtr m_quasistaticReverse{
        m_sysidRoutine.Quasistatic(frc2::sysid::Direction::REVERSE)};
    frc2::CommandPtr m_dynamicForward{
        m_sysidRoutine.Dynamic(frc2::sysid::Direction::FORWARD)};
    frc2::CommandPtr m_dynamicReverse{
        m_sysidRoutine.Dynamic(frc2::sysid::Direction::REVERSE)};
  }

  void TearDown() override { frc::sim::ResumeTiming(); }
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
  std::vector<units::volt_t> expectedVoltages{1_V, 0_V};
  EXPECT_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  EXPECT_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_quasistaticReverse));
  expectedVoltages = std::vector<units::volt_t>{-1_V, 0_V};
  EXPECT_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  EXPECT_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_dynamicForward));
  expectedVoltages = std::vector<units::volt_t>{7_V, 0_V};
  EXPECT_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  EXPECT_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_dynamicReverse));
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
