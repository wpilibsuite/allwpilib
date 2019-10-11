/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/TimedRobot.h>
#include <frc/trajectory/Trajectory.h>
#include <frc/trajectory/TrajectoryGenerator.h>
#include <frc2/command/CommandScheduler.h>

#include "Drivetrain.h"
#include "RamseteTrackerCommand.h"

class Robot : public frc::TimedRobot {
 public:
  // Simple trajectory to make a short left turn.
  const frc::Trajectory m_trajectory =
      frc::TrajectoryGenerator::GenerateTrajectory(
          // Waypoints
          {frc::Pose2d{0_m, 0_m, 0_rad},
           frc::Pose2d{5_ft, 5_ft, frc::Rotation2d(90_deg)}},
          // No additional constraints
          {},
          // Start and end velocities are zero.
          0_mps, 0_mps,
          // Max velocity is 6 feet per second.
          6_fps,
          // Max acceleration is 6 feet per second squared.
          6_fps_sq,
          // The robot will not travel backward.
          false);

  const frc::RamseteController m_controller{2.0, 0.7};

  // Create the auto command
  frc2::Command* command =
      new RamseteTrackerCommand(m_trajectory, m_controller, &drive);

  void AutonomousInit() override {
    // Reset the pose to the position of the first waypoint.
    drive.Reset({});
    command->Schedule();
  }

  void AutonomousPeriodic() override {
    frc2::CommandScheduler::GetInstance().Run();
  }

 private:
  Drivetrain drive;
};

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
