package edu.wpi.first.wpilibj.examples.elevatortrapezoidprofile;

import edu.wpi.first.wpilibj.SpeedController;

/**
 * A simplified stub class that simulates the API of a common "smart" motor controller.
 *
 * Has no actual functioanality.
 */
public class ExampleSmartMotorController implements SpeedController {

  /**
   * Creates a new ExampleSmartMotorController.
   *
   * @param port The port for the controller.
   */
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
   * @param setpoint The controller setpoint.
   * @param arbFeedforward An arbitrary feedforward output (from -1 to 1).
   */
  public void setSetpoint(double setpoint, double arbFeedforward) {
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
