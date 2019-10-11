/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RamseteTrackerCommand.h"

RamseteTrackerCommand::RamseteTrackerCommand(
    frc::Trajectory trajectory, const frc::RamseteController& controller,
    Drivetrain* drivetrain)
    : m_trajectory(std::move(trajectory)),
      m_controller(controller),
      m_drivetrain(drivetrain) {
  // Make the command require the subsystem.
  AddRequirements({m_drivetrain});
}

void RamseteTrackerCommand::Initialize() {
  // Start the timer.
  m_timer.Start();
}

void RamseteTrackerCommand::Execute() {
  // Get the goal point at this time.
  const frc::Trajectory::State goal =
      m_trajectory.Sample(units::second_t(m_timer.Get()));

  // Get the robot position.
  const frc::Pose2d& robotPose = m_drivetrain->GetPose();

  // Update the controller
  const frc::ChassisSpeeds adjustedSpeeds =
      m_controller.Calculate(robotPose, goal);

  // Set the speeds
  m_drivetrain->SetSpeeds(adjustedSpeeds);
}

bool RamseteTrackerCommand::IsFinished() {
  // The tracking is finished when the time elapsed is greater than the
  // trajectory duration.
  return m_timer.Get() > m_trajectory.TotalTime().to<double>();
}

void RamseteTrackerCommand::End(bool interrupted) {
  // Stop the timer.
  m_timer.Stop();
}
