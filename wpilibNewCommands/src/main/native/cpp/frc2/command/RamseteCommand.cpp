/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/RamseteCommand.h"

using namespace frc2;
using namespace units;

RamseteCommand::RamseteCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::RamseteController controller,
    frc::SimpleMotorFeedforward<units::meters> feedforward,
    frc::DifferentialDriveKinematics kinematics,
    std::function<frc::DifferentialDriveWheelSpeeds()> wheelSpeeds,
    frc2::PIDController leftController, frc2::PIDController rightController,
    std::function<void(volt_t, volt_t)> output,
    std::initializer_list<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_controller(controller),
      m_feedforward(feedforward),
      m_kinematics(kinematics),
      m_speeds(wheelSpeeds),
      m_leftController(std::make_unique<frc2::PIDController>(leftController)),
      m_rightController(std::make_unique<frc2::PIDController>(rightController)),
      m_outputVolts(output),
      m_usePID(true) {
  AddRequirements(requirements);
}

RamseteCommand::RamseteCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::RamseteController controller,
    frc::SimpleMotorFeedforward<units::meters> feedforward,
    frc::DifferentialDriveKinematics kinematics,
    std::function<frc::DifferentialDriveWheelSpeeds()> wheelSpeeds,
    frc2::PIDController leftController, frc2::PIDController rightController,
    std::function<void(volt_t, volt_t)> output,
    wpi::ArrayRef<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_controller(controller),
      m_feedforward(feedforward),
      m_kinematics(kinematics),
      m_speeds(wheelSpeeds),
      m_leftController(std::make_unique<frc2::PIDController>(leftController)),
      m_rightController(std::make_unique<frc2::PIDController>(rightController)),
      m_outputVolts(output),
      m_usePID(true) {
  AddRequirements(requirements);
}

RamseteCommand::RamseteCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::RamseteController controller,
    frc::DifferentialDriveKinematics kinematics,
    std::function<void(units::meters_per_second_t, units::meters_per_second_t)>
        output,
    std::initializer_list<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_controller(controller),
      m_kinematics(kinematics),
      m_outputVel(output),
      m_usePID(false) {
  AddRequirements(requirements);
}

RamseteCommand::RamseteCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::RamseteController controller,
    frc::DifferentialDriveKinematics kinematics,
    std::function<void(units::meters_per_second_t, units::meters_per_second_t)>
        output,
    wpi::ArrayRef<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_controller(controller),
      m_kinematics(kinematics),
      m_outputVel(output),
      m_usePID(false) {
  AddRequirements(requirements);
}

void RamseteCommand::Initialize() {
  m_prevTime = -1_s;
  auto initialState = m_trajectory.Sample(0_s);
  m_prevSpeeds = m_kinematics.ToWheelSpeeds(
      frc::ChassisSpeeds{initialState.velocity, 0_mps,
                         initialState.velocity * initialState.curvature});
  m_timer.Reset();
  m_timer.Start();
  if (m_usePID) {
    m_leftController->Reset();
    m_rightController->Reset();
  }
}

void RamseteCommand::Execute() {
  auto curTime = m_timer.Get();
  auto dt = curTime - m_prevTime;

  if (m_prevTime < 0_s) {
    if (m_usePID)
      m_outputVolts(0_V, 0_V);
    else
      m_outputVel(0_mps, 0_mps);

    m_prevTime = curTime;
    return;
  }

  auto targetWheelSpeeds = m_kinematics.ToWheelSpeeds(
      m_controller.Calculate(m_pose(), m_trajectory.Sample(curTime)));

  if (m_usePID) {
    auto leftFeedforward = m_feedforward.Calculate(
        targetWheelSpeeds.left,
        (targetWheelSpeeds.left - m_prevSpeeds.left) / dt);

    auto rightFeedforward = m_feedforward.Calculate(
        targetWheelSpeeds.right,
        (targetWheelSpeeds.right - m_prevSpeeds.right) / dt);

    auto leftOutput = volt_t(m_leftController->Calculate(
                          m_speeds().left.to<double>(),
                          targetWheelSpeeds.left.to<double>())) +
                      leftFeedforward;

    auto rightOutput = volt_t(m_rightController->Calculate(
                           m_speeds().right.to<double>(),
                           targetWheelSpeeds.right.to<double>())) +
                       rightFeedforward;

    m_outputVolts(leftOutput, rightOutput);
  } else {
    m_outputVel(targetWheelSpeeds.left, targetWheelSpeeds.right);
  }
  m_prevSpeeds = targetWheelSpeeds;
  m_prevTime = curTime;
}

void RamseteCommand::End(bool interrupted) { m_timer.Stop(); }

bool RamseteCommand::IsFinished() {
  return m_timer.HasElapsed(m_trajectory.TotalTime());
}
