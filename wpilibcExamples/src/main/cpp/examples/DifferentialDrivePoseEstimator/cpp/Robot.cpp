// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"
#include "wpi/driverstation/Gamepad.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/math/filter/SlewRateLimiter.hpp"

class Robot : public wpi::TimedRobot {
 public:
  void AutonomousPeriodic() override {
    TeleopPeriodic();
    drive.UpdateOdometry();
  }

  void RobotPeriodic() override { drive.Periodic(); }

  void TeleopPeriodic() override {
    // Get the x velocity. We are inverting this because gamepads return
    // negative values when we push forward.
    const auto xVelocity = -velocityLimiter.Calculate(controller.GetLeftY()) *
                           Drivetrain::kMaxVelocity;

    // Get the rate of angular rotation. We are inverting this because we want a
    // positive value when we pull to the left (remember, CCW is positive in
    // mathematics). Gamepads return positive values when you pull to
    // the right by default.
    const auto rot = -rotLimiter.Calculate(controller.GetRightX()) *
                     Drivetrain::kMaxAngularVelocity;

    drive.Drive(xVelocity, rot);
  }

  void SimulationPeriodic() override { drive.SimulationPeriodic(); }

 private:
  wpi::Gamepad controller{0};

  // Slew rate limiters to make joystick inputs more gentle; 1/3 sec from 0
  // to 1.
  wpi::math::SlewRateLimiter<wpi::units::scalar> velocityLimiter{3 / 1_s};
  wpi::math::SlewRateLimiter<wpi::units::scalar> rotLimiter{3 / 1_s};

  Drivetrain drive;
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
