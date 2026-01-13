// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"
#include "wpi/driverstation/XboxController.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/math/controller/LTVUnicycleController.hpp"
#include "wpi/math/filter/SlewRateLimiter.hpp"
#include "wpi/math/trajectory/TrajectoryGenerator.hpp"
#include "wpi/system/Timer.hpp"

class Robot : public wpi::TimedRobot {
 public:
  Robot()
      : m_trajectory(wpi::math::TrajectoryGenerator::GenerateTrajectory(
            wpi::math::Pose2d{2_m, 2_m, 0_rad}, {},
            wpi::math::Pose2d{6_m, 4_m, 0_rad},
            wpi::math::TrajectoryConfig(2_mps, 2_mps_sq))) {}

  void RobotPeriodic() override { m_drive.Periodic(); }

  void AutonomousInit() override {
    m_timer.Restart();
    m_drive.ResetOdometry(m_trajectory.InitialPose());
  }

  void AutonomousPeriodic() override {
    auto elapsed = m_timer.Get();
    auto reference = m_trajectory.SampleAt(elapsed);
    auto speeds = m_feedback.Calculate(m_drive.GetPose(), reference);
    m_drive.Drive(speeds.vx, speeds.omega);
  }

  void TeleopPeriodic() override {
    // Get the x speed. We are inverting this because Xbox controllers return
    // negative values when we push forward.
    const auto xSpeed = -m_speedLimiter.Calculate(m_controller.GetLeftY()) *
                        Drivetrain::kMaxSpeed;

    // Get the rate of angular rotation. We are inverting this because we want a
    // positive value when we pull to the left (remember, CCW is positive in
    // mathematics). Xbox controllers return positive values when you pull to
    // the right by default.
    auto rot = -m_rotLimiter.Calculate(m_controller.GetRightX()) *
               Drivetrain::kMaxAngularSpeed;

    m_drive.Drive(xSpeed, rot);
  }

  void SimulationPeriodic() override { m_drive.SimulationPeriodic(); }

 private:
  wpi::XboxController m_controller{0};

  // Slew rate limiters to make joystick inputs more gentle; 1/3 sec from 0
  // to 1.
  wpi::math::SlewRateLimiter<wpi::units::scalar> m_speedLimiter{3 / 1_s};
  wpi::math::SlewRateLimiter<wpi::units::scalar> m_rotLimiter{3 / 1_s};

  Drivetrain m_drive;
  wpi::math::SplineTrajectory m_trajectory;
  wpi::math::LTVUnicycleController m_feedback{20_ms};
  wpi::Timer m_timer;
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
