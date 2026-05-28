// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.differentialdrive;

import org.wpilib.drive.DifferentialDrive;
import org.wpilib.driverstation.Gamepad;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.motor.PWMSparkMax;

/**
 * DifferentialDrive snippets for wpilib-docs. Runs the motors with tank
 * drive, arcade drive, and curvature drive.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/motors/wpi-drive-classes.html
 */
public class Robot extends TimedRobot {
  private final PWMSparkMax leftMotor = new PWMSparkMax(0);
  private final PWMSparkMax rightMotor = new PWMSparkMax(1);
  private final DifferentialDrive robotDrive =
      new DifferentialDrive(leftMotor::setThrottle, rightMotor::setThrottle);
  private final Gamepad driverController = new Gamepad(0);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    rightMotor.setInverted(true);
  }

  @Override
  public void teleopPeriodic() {
    // Tank drive with a given left and right rates
    robotDrive.tankDrive(-driverController.getLeftY(), -driverController.getRightY());
    // Arcade drive with a given forward and turn rate
    robotDrive.arcadeDrive(-driverController.getLeftY(), -driverController.getLeftX());
    // Curvature drive with a given forward and turn rate, as well as a button for turning in-place.
    robotDrive.curvatureDrive(-driverController.getLeftY(), -driverController.getLeftX(), driverController.getButton(Gamepad.Button.NORTH_FACE));
  }
}
