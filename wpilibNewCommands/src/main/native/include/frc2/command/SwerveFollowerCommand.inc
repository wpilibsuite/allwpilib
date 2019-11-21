/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
#pragma once

template <typename T>
int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

namespace frc2 {

template <size_t NumModules>
SwerveFollowerCommand<NumModules>::SwerveFollowerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SwerveDriveKinematics<NumModules> kinematics,
    frc2::PIDController xController, frc2::PIDController yController,
    frc::ProfiledPIDController thetaController,
    std::function<void(std::array<frc::SwerveModuleState, NumModules>)> output,
    std::initializer_list<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_kinematics(kinematics),
      m_xController(std::make_unique<frc2::PIDController>(xController)),
      m_yController(std::make_unique<frc2::PIDController>(yController)),
      m_thetaController(
          std::make_unique<frc::ProfiledPIDController>(thetaController)),
      m_outputStates(output) {
  this->AddRequirements(requirements);
}

template <size_t NumModules>
void SwerveFollowerCommand<NumModules>::Initialize() {
  m_finalPose = m_trajectory.Sample(m_trajectory.TotalTime()).pose;

  m_timer.Reset();
  m_timer.Start();
}

template <size_t NumModules>
void SwerveFollowerCommand<NumModules>::Execute() {
  auto curTime = units::second_t(m_timer.Get());

  auto m_desiredState = m_trajectory.Sample(curTime);
  auto m_desiredPose = m_desiredState.pose;

  auto m_poseError = m_desiredPose.RelativeTo(m_pose());

  auto targetXVel =
      units::velocity::meters_per_second_t(m_xController->Calculate(
          (m_pose().Translation().X().template to<double>()),
          (m_desiredPose.Translation().X().template to<double>())));
  auto targetYVel =
      units::velocity::meters_per_second_t(m_yController->Calculate(
          (m_pose().Translation().Y().template to<double>()),
          (m_desiredPose.Translation().Y().template to<double>())));

  // Profiled PID Controller only takes meters as setpoint and measurement
  // The robot will go to the desired rotation of the final pose in the
  // trajectory, not following the poses at individual states.
  auto targetAngularVel = units::angular_velocity::radians_per_second_t(
      m_thetaController->Calculate(
          units::meter_t(m_pose().Rotation().Radians().template to<double>()),
          units::meter_t(
              m_finalPose.Rotation().Radians().template to<double>())));

  auto vRef = m_desiredState.velocity;

  targetXVel += vRef * m_poseError.Rotation().Cos();
  targetYVel += vRef * m_poseError.Rotation().Sin();

  auto targetChassisSpeeds =
      frc::ChassisSpeeds{targetXVel, targetYVel, targetAngularVel};

  auto targetModuleStates =
      m_kinematics.ToSwerveModuleStates(targetChassisSpeeds);

  m_outputStates(targetModuleStates);
}

template <size_t NumModules>
void SwerveFollowerCommand<NumModules>::End(bool interrupted) {
  m_timer.Stop();
}

template <size_t NumModules>
bool SwerveFollowerCommand<NumModules>::IsFinished() {
  return m_timer.HasPeriodPassed(m_trajectory.TotalTime());
}

}  // namespace frc2
