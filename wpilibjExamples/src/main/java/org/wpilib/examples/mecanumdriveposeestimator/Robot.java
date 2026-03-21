// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.mecanumdriveposeestimator;

import org.wpilib.driverstation.Gamepad;
import org.wpilib.framework.TimedRobot;
import org.wpilib.math.filter.SlewRateLimiter;

public class Robot extends TimedRobot {
  private final Gamepad controller = new Gamepad(0);
  private final Drivetrain mecanum = new Drivetrain();

  // Slew rate limiters to make joystick inputs more gentle; 1/3 sec from 0 to 1.
  private final SlewRateLimiter xVelocityLimiter = new SlewRateLimiter(3);
  private final SlewRateLimiter yVelocityLimiter = new SlewRateLimiter(3);
  private final SlewRateLimiter rotLimiter = new SlewRateLimiter(3);

  @Override
  public void autonomousPeriodic() {
    driveWithJoystick(false);
    mecanum.updateOdometry();
  }

  @Override
  public void teleopPeriodic() {
    driveWithJoystick(true);
  }

  private void driveWithJoystick(boolean fieldRelative) {
    // Get the x velocity. We are inverting this because gamepads return
    // negative values when we push forward.
    final var xVelocity =
        -xVelocityLimiter.calculate(controller.getLeftY()) * Drivetrain.kMaxVelocity;

    // Get the y velocity or sideways/strafe velocity. We are inverting this because
    // we want a positive value when we pull to the left. Gamepads
    // return positive values when you pull to the right by default.
    final var yVelocity =
        -yVelocityLimiter.calculate(controller.getLeftX()) * Drivetrain.kMaxVelocity;

    // Get the rate of angular rotation. We are inverting this because we want a
    // positive value when we pull to the left (remember, CCW is positive in
    // mathematics). Gamepads return positive values when you pull to
    // the right by default.
    final var rot = -rotLimiter.calculate(controller.getRightX()) * Drivetrain.kMaxAngularVelocity;

    mecanum.drive(xVelocity, yVelocity, rot, fieldRelative, getPeriod());
  }
}
