/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.armbotoffboard;

import edu.wpi.first.wpilibj.SpeedController;

/**
 * A simplified stub class that simulates the API of a common "smart" motor controller.
 *
 * <p>Has no actual functionality.
 */
public class ExampleSmartMotorController implements SpeedController {
  public enum PIDMode {
    kPosition,
    kVelocity,
    kMovementWitchcraft
  }

  /**
   * Creates a new ExampleSmartMotorController.
   *
   * @param port The port for the controller.
   */
  @SuppressWarnings("PMD.UnusedFormalParameter")
  public ExampleSmartMotorController(int port) {
  }

  /**
   * Example method for setting the PID gains of the smart controller.
   *
   * @param kp The proportional gain.
   * @param ki The integral gain.
   * @param kd The derivative gain.
   */
  public void setPID(double kp, double ki, double kd) {
  }

  /**
   * Example method for setting the setpoint of the smart controller in PID mode.
   *
   * @param mode The mode of the PID controller.
   * @param setpoint The controller setpoint.
   * @param arbFeedforward An arbitrary feedforward output (from -1 to 1).
   */
  public void setSetpoint(PIDMode mode, double setpoint, double arbFeedforward) {
  }

  /**
   * Places this motor controller in follower mode.
   *
   * @param leader The leader to follow.
   */
  public void follow(ExampleSmartMotorController leader) {
  }

  /**
   * Returns the encoder distance.
   *
   * @return The current encoder distance.
   */
  public double getEncoderDistance() {
    return 0;
  }

  /**
   * Returns the encoder rate.
   *
   * @return The current encoder rate.
   */
  public double getEncoderRate() {
    return 0;
  }

  /**
   * Resets the encoder to zero distance.
   */
  public void resetEncoder() {
  }

  @Override
  public void set(double speed) {
  }

  @Override
  public double get() {
    return 0;
  }

  @Override
  public void setInverted(boolean isInverted) {

  }

  @Override
  public boolean getInverted() {
    return false;
  }

  @Override
  public void disable() {
  }

  @Override
  public void stopMotor() {
  }

  @Override
  public void pidWrite(double output) {
  }
}
