// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"
#include "wpi/driverstation/XboxController.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/math/filter/SlewRateLimiter.hpp"
#include "wpi/math/util/MathUtil.hpp"

class Robot : public wpi::TimedRobot {
 public:
  void AutonomousPeriodic() override {
    DriveWithJoystick(false);
    m_swerve.UpdateOdometry();
  }

  void TeleopPeriodic() override { DriveWithJoystick(true); }

 private:
  wpi::XboxController m_controller{0};
  Drivetrain m_swerve;

  // Slew rate limiters to make joystick inputs more gentle; 1/3 sec from 0
  // to 1.
  wpi::math::SlewRateLimiter<wpi::units::scalar> m_xVelocityLimiter{3 / 1_s};
  wpi::math::SlewRateLimiter<wpi::units::scalar> m_yVelocityLimiter{3 / 1_s};
  wpi::math::SlewRateLimiter<wpi::units::scalar> m_rotLimiter{3 / 1_s};

  void DriveWithJoystick(bool fieldRelative) {
    // Get the x velocity. We are inverting this because Xbox controllers return
    // negative values when we push forward.
    const auto xVelocity =
        -m_xVelocityLimiter.Calculate(
            wpi::math::ApplyDeadband(m_controller.GetLeftY(), 0.02)) *
        Drivetrain::kMaxVelocity;

    // Get the y velocity or sideways/strafe velocity. We are inverting this
    // because we want a positive value when we pull to the left. Xbox
    // controllers return positive values when you pull to the right by default.
    const auto yVelocity =
        -m_yVelocityLimiter.Calculate(
            wpi::math::ApplyDeadband(m_controller.GetLeftX(), 0.02)) *
        Drivetrain::kMaxVelocity;

    // Get the rate of angular rotation. We are inverting this because we want a
    // positive value when we pull to the left (remember, CCW is positive in
    // mathematics). Xbox controllers return positive values when you pull to
    // the right by default.
    const auto rot = -m_rotLimiter.Calculate(wpi::math::ApplyDeadband(
                         m_controller.GetRightX(), 0.02)) *
                     Drivetrain::kMaxAngularVelocity;

    m_swerve.Drive(xVelocity, yVelocity, rot, fieldRelative, GetPeriod());
  }
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
