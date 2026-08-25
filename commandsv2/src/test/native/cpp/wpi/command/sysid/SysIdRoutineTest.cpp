// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/sysid/SysIdRoutine.hpp"

#include <algorithm>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/commands2/Subsystem.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/system/DataLogManager.hpp"
#include "wpi/system/Timer.hpp"

#define CHECK_NEAR_UNITS(val1, val2, eps) \
  CHECK(wpi::units::abs(val1 - val2) <= eps)

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

class SysIdRoutineTest {
 public:
  SysIdRoutineTest() { wpi::sim::PauseTiming(); }

  ~SysIdRoutineTest() {
    wpi::sim::ResumeTiming();
    wpi::DataLogManager::Stop();
  }

  std::vector<StateTest> currentStateList{};
  std::vector<wpi::units::volts<>> sentVoltages{};
  wpi::cmd::Subsystem m_subsystem{};
  wpi::cmd::sysid::SysIdRoutine m_sysidRoutine{
      wpi::cmd::sysid::Config{
          std::nullopt, std::nullopt, std::nullopt,
          [this](wpi::sysid::State state) {
            switch (state) {
              case wpi::sysid::State::QUASISTATIC_FORWARD:
                currentStateList.emplace_back(StateTest::InRecordStateQf);
                break;
              case wpi::sysid::State::QUASISTATIC_REVERSE:
                currentStateList.emplace_back(StateTest::InRecordStateQr);
                break;
              case wpi::sysid::State::DYNAMIC_FORWARD:
                currentStateList.emplace_back(StateTest::InRecordStateDf);
                break;
              case wpi::sysid::State::DYNAMIC_REVERSE:
                currentStateList.emplace_back(StateTest::InRecordStateDr);
                break;
              case wpi::sysid::State::NONE:
                currentStateList.emplace_back(StateTest::DoneWithRecordState);
                break;
            }
          }},
      wpi::cmd::sysid::Mechanism{
          [this](wpi::units::volts<> driveVoltage) {
            sentVoltages.emplace_back(driveVoltage);
            currentStateList.emplace_back(StateTest::InDrive);
          },
          [this](wpi::sysid::SysIdRoutineLog* log) {
            currentStateList.emplace_back(StateTest::InLog);
            log->Motor("Mock Motor").position(0_m).velocity(0_mps).voltage(0_V);
          },
          &m_subsystem}};

  wpi::cmd::CommandPtr m_quasistaticForward{
      m_sysidRoutine.Quasistatic(wpi::cmd::sysid::Direction::FORWARD)};
  wpi::cmd::CommandPtr m_quasistaticReverse{
      m_sysidRoutine.Quasistatic(wpi::cmd::sysid::Direction::REVERSE)};
  wpi::cmd::CommandPtr m_dynamicForward{
      m_sysidRoutine.Dynamic(wpi::cmd::sysid::Direction::FORWARD)};
  wpi::cmd::CommandPtr m_dynamicReverse{
      m_sysidRoutine.Dynamic(wpi::cmd::sysid::Direction::REVERSE)};

  wpi::cmd::sysid::SysIdRoutine m_emptySysidRoutine{
      wpi::cmd::sysid::Config{std::nullopt, std::nullopt, std::nullopt,
                              nullptr},
      wpi::cmd::sysid::Mechanism{[](wpi::units::volts<> driveVoltage) {},
                                 nullptr, &m_subsystem}};

  wpi::cmd::CommandPtr m_emptyRoutineForward{
      m_emptySysidRoutine.Quasistatic(wpi::cmd::sysid::Direction::FORWARD)};

  void RunCommand(wpi::cmd::CommandPtr command) {
    command.get()->Initialize();
    command.get()->Execute();
    wpi::sim::StepTiming(1_s);
    command.get()->Execute();
    command.get()->End(true);
  }
};

TEST_CASE_METHOD(SysIdRoutineTest,
                 "SysIdRoutineTest RecordStateBookendsMotorLogging",
                 "[commandsv2][command]") {
  RunCommand(std::move(m_quasistaticForward));
  std::vector<StateTest> expectedOrder{
      StateTest::InDrive, StateTest::InLog, StateTest::InRecordStateQf,
      StateTest::InDrive, StateTest::DoneWithRecordState};
  CHECK(expectedOrder == currentStateList);
  currentStateList.clear();
  sentVoltages.clear();

  expectedOrder = std::vector<StateTest>{
      StateTest::InDrive, StateTest::InLog, StateTest::InRecordStateDf,
      StateTest::InDrive, StateTest::DoneWithRecordState};
  RunCommand(std::move(m_dynamicForward));
  CHECK(expectedOrder == currentStateList);
  currentStateList.clear();
  sentVoltages.clear();
}

TEST_CASE_METHOD(SysIdRoutineTest, "SysIdRoutineTest DeclareCorrectState",
                 "[commandsv2][command]") {
  RunCommand(std::move(m_quasistaticForward));
  CHECK(std::find(currentStateList.begin(), currentStateList.end(),
                  StateTest::InRecordStateQf) != currentStateList.end());
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_quasistaticReverse));
  CHECK(std::find(currentStateList.begin(), currentStateList.end(),
                  StateTest::InRecordStateQr) != currentStateList.end());
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_dynamicForward));
  CHECK(std::find(currentStateList.begin(), currentStateList.end(),
                  StateTest::InRecordStateDf) != currentStateList.end());
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_dynamicReverse));
  CHECK(std::find(currentStateList.begin(), currentStateList.end(),
                  StateTest::InRecordStateDr) != currentStateList.end());
  currentStateList.clear();
  sentVoltages.clear();
}

TEST_CASE_METHOD(SysIdRoutineTest, "SysIdRoutineTest OutputCorrectVoltage",
                 "[commandsv2][command]") {
  RunCommand(std::move(m_quasistaticForward));
  std::vector<wpi::units::volts<>> expectedVoltages{1_V, 0_V};
  CHECK_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  CHECK_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_quasistaticReverse));
  expectedVoltages = std::vector<wpi::units::volts<>>{-1_V, 0_V};
  CHECK_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  CHECK_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_dynamicForward));
  expectedVoltages = std::vector<wpi::units::volts<>>{7_V, 0_V};
  CHECK_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  CHECK_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();

  RunCommand(std::move(m_dynamicReverse));
  expectedVoltages = std::vector<wpi::units::volts<>>{-7_V, 0_V};
  CHECK_NEAR_UNITS(expectedVoltages[0], sentVoltages[0], 1e-6_V);
  CHECK_NEAR_UNITS(expectedVoltages[1], sentVoltages[1], 1e-6_V);
  currentStateList.clear();
  sentVoltages.clear();
}

TEST_CASE_METHOD(SysIdRoutineTest, "SysIdRoutineTest EmptyLogFunc",
                 "[commandsv2][command]") {
  RunCommand(std::move(m_emptyRoutineForward));
  SUCCEED();
}
