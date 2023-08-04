// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <utility>

#include "frc2/command/SwerveControllerCommand.h"

namespace frc2 {

template <size_t NumModules>
SwerveControllerCommand<NumModules>::SwerveControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SwerveDriveKinematics<NumModules> kinematics,
    frc::PIDController xController, frc::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    std::function<frc::Rotation2d()> desiredRotation,
    std::function<void(std::array<frc::SwerveModuleState, NumModules>)> output,
    Requirements requirements)
    : m_trajectory(std::move(trajectory)),
      m_pose(std::move(pose)),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_desiredRotation(std::move(desiredRotation)),
      m_outputStates(output) {
  this->AddRequirements(requirements);
}

template <size_t NumModules>
SwerveControllerCommand<NumModules>::SwerveControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SwerveDriveKinematics<NumModules> kinematics,
    frc::PIDController xController, frc::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    std::function<void(std::array<frc::SwerveModuleState, NumModules>)> output,
    Requirements requirements)
    : m_trajectory(std::move(trajectory)),
      m_pose(std::move(pose)),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_outputStates(output) {
  this->AddRequirements(requirements);
}

template <size_t NumModules>
SwerveControllerCommand<NumModules>::SwerveControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SwerveDriveKinematics<NumModules> kinematics,
    frc::HolonomicDriveController controller,
    std::function<frc::Rotation2d()> desiredRotation,
    std::function<void(std::array<frc::SwerveModuleState, NumModules>)> output,
    Requirements requirements)
    : m_trajectory(std::move(trajectory)),
      m_pose(std::move(pose)),
      m_kinematics(kinematics),
      m_controller(std::move(controller)),
      m_desiredRotation(std::move(desiredRotation)),
      m_outputStates(output) {
  this->AddRequirements(requirements);
}

template <size_t NumModules>
SwerveControllerCommand<NumModules>::SwerveControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SwerveDriveKinematics<NumModules> kinematics,
    frc::HolonomicDriveController controller,
    std::function<void(std::array<frc::SwerveModuleState, NumModules>)> output,
    Requirements requirements)
    : m_trajectory(std::move(trajectory)),
      m_pose(std::move(pose)),
      m_kinematics(kinematics),
      m_controller(std::move(controller)),
      m_outputStates(output) {
  this->AddRequirements(requirements);
}

template <size_t NumModules>
void SwerveControllerCommand<NumModules>::Initialize() {
  if (m_desiredRotation == nullptr) {
    m_desiredRotation = [&] {
      return m_trajectory.States().back().pose.Rotation();
    };
  }
  m_timer.Restart();
}

template <size_t NumModules>
void SwerveControllerCommand<NumModules>::Execute() {
  auto curTime = m_timer.Get();
  auto m_desiredState = m_trajectory.Sample(curTime);

  auto targetChassisSpeeds =
      m_controller.Calculate(m_pose(), m_desiredState, m_desiredRotation());
  auto targetModuleStates =
      m_kinematics.ToSwerveModuleStates(targetChassisSpeeds);

  m_outputStates(targetModuleStates);
}

template <size_t NumModules>
void SwerveControllerCommand<NumModules>::End(bool interrupted) {
  m_timer.Stop();
}

template <size_t NumModules>
bool SwerveControllerCommand<NumModules>::IsFinished() {
  return m_timer.HasElapsed(m_trajectory.TotalTime());
}

}  // namespace frc2
