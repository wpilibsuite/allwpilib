// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/TrajectoryCommand.h"

#include <utility>

using namespace frc2;

TrajectoryCommand::TrajectoryCommand(
    frc::Trajectory trajectory,
    std::function<void(const frc::Trajectory::State&)> output,
    std::initializer_list<Subsystem* const> requirements)
    : m_trajectory(std::move(trajectory)), m_output(std::move(output)) {
  AddRequirements(requirements);
}

TrajectoryCommand::TrajectoryCommand(
    frc::Trajectory trajectory,
    std::function<void(const frc::Trajectory::State&)> output,
    wpi::span<Subsystem* const> requirements)
    : m_trajectory(std::move(trajectory)), m_output(std::move(output)) {
  AddRequirements(requirements);
}

void TrajectoryCommand::Initialize() {
  m_timer.Reset();
  m_timer.Start();
}

void TrajectoryCommand::Execute() {
  m_output(m_trajectory.Sample(m_timer.Get()));
}

void TrajectoryCommand::End(bool interrupted) {
  m_timer.Stop();

  if (interrupted) {
    m_output(frc::Trajectory::State());
  }
}

bool TrajectoryCommand::IsFinished() {
  return m_timer.HasElapsed(m_trajectory.TotalTime());
}
