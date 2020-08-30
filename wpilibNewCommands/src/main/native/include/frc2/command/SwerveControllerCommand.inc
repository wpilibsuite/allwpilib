/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

namespace frc2 {

template <size_t NumModules>
SwerveControllerCommand<NumModules>::SwerveControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SwerveDriveKinematics<NumModules> kinematics,
    frc2::PIDController xController, frc2::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    std::function<frc::Rotation2d()> desiredRotation,
    std::function<void(std::array<frc::SwerveModuleState, NumModules>)> output,
    std::initializer_list<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_desiredRotation(desiredRotation),
      m_outputStates(output) {
  this->AddRequirements(requirements);
}

template <size_t NumModules>
SwerveControllerCommand<NumModules>::SwerveControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SwerveDriveKinematics<NumModules> kinematics,
    frc2::PIDController xController, frc2::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    std::function<void(std::array<frc::SwerveModuleState, NumModules>)> output,
    std::initializer_list<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_outputStates(output) {
  this->AddRequirements(requirements);
  m_desiredRotation = [&] {
    return m_trajectory.States().back().pose.Rotation();
  };
}

template <size_t NumModules>
SwerveControllerCommand<NumModules>::SwerveControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SwerveDriveKinematics<NumModules> kinematics,
    frc2::PIDController xController, frc2::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    std::function<frc::Rotation2d()> desiredRotation,
    std::function<void(std::array<frc::SwerveModuleState, NumModules>)> output,
    wpi::ArrayRef<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_desiredRotation(desiredRotation),
      m_outputStates(output) {
  this->AddRequirements(requirements);
}

template <size_t NumModules>
SwerveControllerCommand<NumModules>::SwerveControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SwerveDriveKinematics<NumModules> kinematics,
    frc2::PIDController xController, frc2::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    std::function<void(std::array<frc::SwerveModuleState, NumModules>)> output,
    wpi::ArrayRef<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_outputStates(output) {
  this->AddRequirements(requirements);
  m_desiredRotation = [&] {
    return m_trajectory.States().back().pose.Rotation();
  };
}

template <size_t NumModules>
void SwerveControllerCommand<NumModules>::Initialize() {
  m_timer.Reset();
  m_timer.Start();
}

template <size_t NumModules>
void SwerveControllerCommand<NumModules>::Execute() {
  auto curTime = units::second_t(m_timer.Get());
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
