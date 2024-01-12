// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/sysid/SysIdRoutine.h"

using namespace frc2::sysid;

frc2::CommandPtr SysIdRoutine::Quasistatic(Direction direction) {
  std::unordered_map<Direction, frc::sysid::State> stateOptions{
      {Direction::kForward, frc::sysid::State::kQuasistaticForward},
      {Direction::kReverse, frc::sysid::State::kQuasistaticReverse},
  };
  frc::sysid::State state = stateOptions[direction];
  double outputSign = direction == Direction::kForward ? 1.0 : -1.0;

  return m_mechanism.m_subsystem
      ->RunOnce([this] {
        timer.Reset();
        timer.Start();
      })
      .AndThen(
          m_mechanism.m_subsystem
              ->Run([this, state, outputSign] {
                m_outputVolts = outputSign * timer.Get() * m_config.m_rampRate;
                m_mechanism.m_drive(m_outputVolts);
                m_mechanism.m_log(this);
                m_recordState(state);
              })
              .FinallyDo([this] {
                m_mechanism.m_drive(0_V);
                m_recordState(frc::sysid::State::kNone);
                timer.Stop();
              })
              .WithName("sysid-" +
                        frc::sysid::SysIdRoutineLog::StateEnumToString(state) +
                        "-" + m_mechanism.m_name)
              .WithTimeout(m_config.m_timeout));
}

frc2::CommandPtr SysIdRoutine::Dynamic(Direction direction) {
  std::unordered_map<Direction, frc::sysid::State> stateOptions{
      {Direction::kForward, frc::sysid::State::kDynamicForward},
      {Direction::kReverse, frc::sysid::State::kDynamicReverse},
  };
  frc::sysid::State state = stateOptions[direction];
  double outputSign = direction == Direction::kForward ? 1.0 : -1.0;

  return m_mechanism.m_subsystem
      ->RunOnce([this] { m_outputVolts = m_config.m_stepVoltage; })
      .AndThen(m_mechanism.m_subsystem->Run([this, state, outputSign] {
        m_mechanism.m_drive(m_outputVolts * outputSign);
        m_mechanism.m_log(this);
        m_recordState(state);
      }))
      .FinallyDo([this] {
        m_mechanism.m_drive(0_V);
        m_recordState(frc::sysid::State::kNone);
      })
      .WithName("sysid-" +
                frc::sysid::SysIdRoutineLog::StateEnumToString(state) + "-" +
                m_mechanism.m_name)
      .WithTimeout(m_config.m_timeout);
}
