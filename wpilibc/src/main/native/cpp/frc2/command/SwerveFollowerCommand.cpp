/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/SwerveFollowerCommand.h"

using namespace frc2;
using namespace units;

template <typename T>
int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

SwerveFollowerCommand::SwerveFollowerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SwerveDriveKinematics <4> kinematics,
    frc2::PIDController xController, frc2::PIDController yController, frc::ProfiledPIDController thetaController,
    std::function<void(frc::SwerveModuleState, frc::SwerveModuleState, frc::SwerveModuleState, frc::SwerveModuleState)>
        output,
    std::initializer_list<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_kinematics(kinematics),
      m_xController(std::make_unique<frc2::PIDController>(xController)),
      m_yController(std::make_unique<frc2::PIDController>(yController)),
      m_thetaController(std::make_unique<frc::ProfiledPIDController>(thetaController)),
      m_outputStates(output) {
  AddRequirements(requirements);
}


void SwerveFollowerCommand::Initialize() {
  auto m_finalPose = m_trajectory.Sample(m_trajectory.TotalTime).pose;
  
  m_timer.Reset();
  m_timer.Start();
}

void SwerveFollowerCommand::Execute() {
  auto curTime = second_t(m_timer.Get());

  auto m_desiredState = m_trajectory.Sample(curTime);
  auto m_desiredPose = m_desiredState.pose;

  auto m_poseError = m_desiredPose.RelativeTo(m_pose());

  auto targetXVel = velocity::meters_per_second_t(m_xController->Calculate((m_pose().Translation().X().to<double>()), (m_desiredPose.Translation().X().to<double>())));
  auto targetYVel = velocity::meters_per_second_t(m_yController->Calculate((m_pose().Translation().Y().to<double>()), (m_desiredPose.Translation().Y().to<double>())));
  auto targetAngularVel = angular_velocity::radians_per_second_t(m_thetaController->Calculate((m_pose().Rotation().Radians().to<double>()), (m_finalPose.Rotation().Radians().to<double>())));

  auto vRef = m_desiredState.velocity;
    
  targetXVel += vRef * sin (m_poseError.Rotation().Radians().to<double>());
  targetYVel += vRef * cos (m_poseError.Rotation().Radians().to<double>());
  
  auto targetChassisSpeeds = frc::ChassisSpeeds{targetXVel, targetYVel, targetAngularVel};

  auto targetModuleStates = m_kinematics.ToSwerveModuleStates(targetChassisSpeeds);

  auto frontLeftOutput = targetModuleStates[0];
  auto rearLeftOutput = targetModuleStates[1];
  auto frontRightOutput = targetModuleStates[2];
  auto rearRightOutput = targetModuleStates[3]; 


  m_outputStates(frontLeftOutput, rearLeftOutput, frontRightOutput, rearRightOutput);

}

void SwerveFollowerCommand::End(bool interrupted) { m_timer.Stop(); }

bool SwerveFollowerCommand::IsFinished() {
  return second_t(m_timer.Get()) - m_trajectory.TotalTime() >= 0_s;
}
