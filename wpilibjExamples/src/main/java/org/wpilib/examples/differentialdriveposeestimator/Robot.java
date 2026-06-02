// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.differentialdriveposeestimator;

import org.wpilib.driverstation.Gamepad;
import org.wpilib.framework.TimedRobot;
import org.wpilib.math.filter.SlewRateLimiter;
import org.wpilib.networktables.DoubleArrayTopic;
import org.wpilib.networktables.NetworkTableInstance;

public class Robot extends TimedRobot {
  private final NetworkTableInstance inst = NetworkTableInstance.getDefault();
  private final DoubleArrayTopic doubleArrayTopic = inst.getDoubleArrayTopic("doubleArrayTopic");

  private final Gamepad controller = new Gamepad(0);
  private final Drivetrain drive = new Drivetrain(doubleArrayTopic);

  // Slew rate limiters to make joystick inputs more gentle; 1/3 sec from 0 to 1.
  private final SlewRateLimiter velocityLimiter = new SlewRateLimiter(3);
  private final SlewRateLimiter rotLimiter = new SlewRateLimiter(3);

  @Override
  public void autonomousPeriodic() {
    teleopPeriodic();
    drive.updateOdometry();
  }

  @Override
  public void simulationPeriodic() {
    drive.simulationPeriodic();
  }

  @Override
  public void robotPeriodic() {
    drive.periodic();
  }

  @Override
  public void teleopPeriodic() {
    // Get the x velocity. We are inverting this because gamepad return
    // negative values when we push forward.
    final var xVelocity =
        -velocityLimiter.calculate(controller.getLeftY()) * Drivetrain.kMaxVelocity;

    // Get the rate of angular rotation. We are inverting this because we want a
    // positive value when we pull to the left (remember, CCW is positive in
    // mathematics). Gamepads return positive values when you pull to
    // the right by default.
    final var rot = -rotLimiter.calculate(controller.getRightX()) * Drivetrain.kMaxAngularVelocity;

    drive.drive(xVelocity, rot);
  }
}
