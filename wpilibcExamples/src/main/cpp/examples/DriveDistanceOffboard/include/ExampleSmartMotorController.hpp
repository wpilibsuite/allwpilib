// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

/**
 * A simplified stub class that simulates the API of a common "smart" motor
 * controller.
 *
 * <p>Has no actual functionality.
 */
class ExampleSmartMotorController {
 public:
  enum PIDMode { kPosition, kVelocity, kMovementWitchcraft };

  /**
   * Creates a new ExampleSmartMotorController.
   *
   * @param port The port for the controller.
   */
  explicit ExampleSmartMotorController(int port) {}

  /**
   * Example method for setting the PID gains of the smart controller.
   *
   * @param kp The proportional gain.
   * @param ki The integral gain.
   * @param kd The derivative gain.
   */
  void SetPID(double kp, double ki, double kd) {}

  /**
   * Example method for setting the setpoint of the smart controller in PID
   * mode.
   *
   * @param mode The mode of the PID controller.
   * @param setpoint The controller setpoint.
   * @param arbFeedforward An arbitrary feedforward output (from -1 to 1).
   */
  void SetSetpoint(PIDMode mode, double setpoint, double arbFeedforward) {}

  /**
   * Places this motor controller in follower mode.
   *
   * @param leader The leader to follow.
   */
  void Follow(ExampleSmartMotorController leader) {}

  /**
   * Returns the encoder distance.
   *
   * @return The current encoder distance.
   */
  double GetEncoderDistance() { return 0; }

  /**
   * Returns the encoder rate.
   *
   * @return The current encoder rate.
   */
  double GetEncoderRate() { return 0; }

  /**
   * Resets the encoder to zero distance.
   */
  void ResetEncoder() {}

  void Set(double speed) { m_value = speed; }

  double Get() const { return m_value; }

  void SetInverted(bool isInverted) {}

  bool GetInverted() const { return false; }

  void Disable() {}

  void StopMotor() {}

 private:
  double m_value = 0.0;
};
