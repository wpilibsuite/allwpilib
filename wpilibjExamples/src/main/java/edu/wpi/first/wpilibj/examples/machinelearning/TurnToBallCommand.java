/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.machinelearning;

import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj2.command.PIDCommand;
import edu.wpi.first.wpilibj.examples.machinelearning.DriveSubsystem;
import edu.wpi.first.wpilibj.examples.machinelearning.VisionSubsystem;

/**
 * Command that turns the robot to face the first detected hatch
 */
public class TurnToBallCommand extends PIDCommand {
  private VisionSubsystem visionSubsystem;
  private DriveSubsystem driveSubsystem;

  /**
   * PIDCommand uses relative hatch angle at time of initialization, not construction.
   * Turn is based on gyro.
   * @param driveSubsystem the drive subsystem
   * @param visionSubsystem the vision subsystem
   */
  public TurnToBallCommand(DriveSubsystem driveSubsystem, VisionSubsystem visionSubsystem) {
    super(
        // Tune these values for your chassis
        new PIDController(1, 0, 0),
        // Gets the heading of the robot in radians as PID input
        () -> driveSubsystem.getAngle().getRadians(),
        // 0 setpoint at construction, immediately overwritten at time of init
        0.0,
        // Turns with output
        (double value) -> driveSubsystem.drive(0, value),
        // Required subsystems
        driveSubsystem,
        visionSubsystem
    );
    this.driveSubsystem = driveSubsystem;
    this.visionSubsystem = visionSubsystem;
    // Set tolerance for PID
    getController().setTolerance(10, 5);
    // Using gyro heading in Radians, so continuous input
    getController().enableContinuousInput(-Math.PI, Math.PI);
  }

  /**
   * Sets setpoint at init
   */
  @Override
  public void initialize() {
    super.initialize();
    getController().setSetpoint(getBallAngle());
  }

  @Override
  public boolean isFinished() {
    return getController().atSetpoint();
  }

  /**
   * Gets the angle of the hatch if there is one
   * @return angle of hatch
   */
  public double getBallAngle() {
    if (visionSubsystem.getTotalBalls() > 0) {
      return visionSubsystem.getBalls()[0].getAngle();
    }
    // return current drivetrain angle if no hatch detected, so no turning
    return driveSubsystem.getAngle().getRadians();
  }
}
