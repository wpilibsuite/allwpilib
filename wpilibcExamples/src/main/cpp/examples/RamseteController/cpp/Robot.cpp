/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/SlewRateLimiter.h>
#include <frc/TimedRobot.h>
#include <frc/XboxController.h>
#include <frc/controller/RamseteController.h>
#include <frc/trajectory/TrajectoryGenerator.h>
#include <frc2/Timer.h>

#include "Drivetrain.h"

class Robot : public frc::TimedRobot {
 public:
  void AutonomousInit() override {
    // Start the timer.
    m_timer.Start();

    // Reset the drivetrain's odometry to the starting pose of the trajectory.
    m_drive.ResetOdometry(m_trajectory.InitialPose());
  }

  void AutonomousPeriodic() override {
    // Update odometry.
    m_drive.UpdateOdometry();

    if (m_timer.Get() < m_trajectory.TotalTime()) {
      // Get the desired pose from the trajectory.
      auto desiredPose = m_trajectory.Sample(m_timer.Get());

      // Get the reference chassis speeds from the Ramsete Controller.
      auto refChassisSpeeds =
          m_ramseteController.Calculate(m_drive.GetPose(), desiredPose);

      // Set the linear and angular speeds.
      m_drive.Drive(refChassisSpeeds.vx, refChassisSpeeds.omega);
    } else {
      m_drive.Drive(0_mps, 0_rad_per_s);
    }
  }

  void TeleopPeriodic() override {
    // Get the x speed. We are inverting this because Xbox controllers return
    // negative values when we push forward.
    const auto xSpeed = -m_speedLimiter.Calculate(
                            m_controller.GetY(frc::GenericHID::kLeftHand)) *
                        Drivetrain::kMaxSpeed;

    // Get the rate of angular rotation. We are inverting this because we want a
    // positive value when we pull to the left (remember, CCW is positive in
    // mathematics). Xbox controllers return positive values when you pull to
    // the right by default.
    const auto rot = -m_rotLimiter.Calculate(
                         m_controller.GetX(frc::GenericHID::kRightHand)) *
                     Drivetrain::kMaxAngularSpeed;

    m_drive.Drive(xSpeed, rot);
  }

 private:
  frc::XboxController m_controller{0};

  // Slew rate limiters to make joystick inputs more gentle; 1/3 sec from 0
  // to 1.
  frc::SlewRateLimiter<units::scalar> m_speedLimiter{3 / 1_s};
  frc::SlewRateLimiter<units::scalar> m_rotLimiter{3 / 1_s};

  Drivetrain m_drive;

  // An example trajectory to follow.
  frc::Trajectory m_trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      frc::Pose2d(0_m, 0_m, 0_rad),
      {frc::Translation2d(1_m, 1_m), frc::Translation2d(2_m, -1_m)},
      frc::Pose2d(3_m, 0_m, 0_rad), frc::TrajectoryConfig(3_fps, 3_fps_sq));

  // The Ramsete Controller to follow the trajectory.
  frc::RamseteController m_ramseteController;

  // The timer to use during the autonomous period.
  frc2::Timer m_timer;
};

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
