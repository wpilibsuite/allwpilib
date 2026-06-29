// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.mechanisms;

import org.wpilib.epilogue.Logged;
import org.wpilib.examples.rebuiltcmdv3.stubs.ExampleSmartMotorController;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.SwerveModulePosition;
import org.wpilib.math.kinematics.SwerveModuleVelocity;

@Logged
public final class SwerveModule {
  private final ExampleSmartMotorController driveMotor;
  private final ExampleSmartMotorController turnMotor;

  /**
   * Creates a new swerve module with the given drive and turn motors. This constructor is
   * package-private and should only be called by the SwerveDrive class.
   *
   * @param driveMotor The motor that drives the module.
   * @param turnMotor The motor that turns the module.
   */
  SwerveModule(ExampleSmartMotorController driveMotor, ExampleSmartMotorController turnMotor) {
    this.driveMotor = driveMotor;
    this.turnMotor = turnMotor;
  }

  /**
   * Sets the target velocity of this module.
   *
   * @param targetVelocity The target velocity of the module.
   */
  public void setTargetVelocity(SwerveModuleVelocity targetVelocity) {
    driveMotor.setSetpoint(ExampleSmartMotorController.PIDMode.kVelocity, targetVelocity.velocity);
    turnMotor.setSetpoint(
        ExampleSmartMotorController.PIDMode.kPosition, targetVelocity.angle.getRadians());
  }

  /** Stops the module by turning off the drive and turn motors. */
  public void stop() {
    driveMotor.stopMotor();
    turnMotor.stopMotor();
  }

  /**
   * Gets the current position of the module.
   *
   * @return The current position of the module.
   */
  public SwerveModulePosition getPosition() {
    return new SwerveModulePosition(
        driveMotor.getEncoderDistance(), Rotation2d.fromRadians(turnMotor.getEncoderDistance()));
  }

  /**
   * Gets the current velocity of the module.
   *
   * @return The current velocity of the module.
   */
  public SwerveModuleVelocity getVelocity() {
    return new SwerveModuleVelocity(
        driveMotor.getEncoderRate(), Rotation2d.fromRadians(turnMotor.getEncoderDistance()));
  }
}
