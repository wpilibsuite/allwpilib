/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.ctrlsys;

import edu.wpi.first.wpilibj.HLUsageReporting;
import edu.wpi.first.wpilibj.PIDInterface;
import edu.wpi.first.wpilibj.PIDOutput;
import edu.wpi.first.wpilibj.SendableBase;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * Class implements a PID Control Loop.
 *
 * <p>Creates a separate thread which reads the given PIDSource and takes care of the integral
 * calculations, as well as writing the given PIDOutput.
 *
 * <p>This feedback controller runs in discrete time, so time deltas are not used in the integral
 * and derivative calculations. Therefore, the sample rate affects the controller's behavior for a
 * given set of PID constants.
 */
public class PIDController extends SendableBase implements PIDInterface {
  private static int instances = 0;

  private RefInput m_refInput = new RefInput(0.0);
  private GainNode m_feedforward;
  private SumNode m_sum;
  private PIDNode m_pid;
  private Output m_output;
  private boolean m_enabled = false;

  /**
   * Allocate a PID object with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param input  the node that is used to get values
   * @param output The PIDOutput object that is set to the output value
   * @param period the loop time for doing calculations. This particularly
   *               effects calculations of the integral and differential terms.
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, INode input,
                PIDOutput output, double period) {
    m_sum = new SumNode(m_refInput, true, input, false);
    m_pid = new PIDNode(Kp, Ki, Kd, m_sum, period);
    m_output = new Output(m_pid, output, period);

    instances++;
    HLUsageReporting.reportPIDController(instances);
  }

  /**
   * Allocate a PID object with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param input  the node that is used to get values
   * @param output The PIDOutput object that is set to the output value
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, INode input,
                PIDOutput output) {
    this(Kp, Ki, Kd, input, output, INode.DEFAULT_PERIOD);
  }

  /**
   * Allocate a PID object with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param Kff    the feedforward coefficient
   * @param input  the node that is used to get values
   * @param output The PIDOutput object that is set to the output value
   * @param period the loop time for doing calculations. This particularly
   *               effects calculations of the integral and differential terms.
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, double Kff,
                INode input, PIDOutput output, double period) {
    m_feedforward = new GainNode(Kff, m_refInput);
    m_sum = new SumNode(m_refInput, true, input, false);
    m_pid = new PIDNode(Kp, Ki, Kd, m_feedforward, m_sum, period);
    m_output = new Output(m_pid, output, period);

    instances++;
    HLUsageReporting.reportPIDController(instances);
  }

  /**
   * Allocate a PID object with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param Kff    the feedforward coefficient
   * @param input  the node that is used to get values
   * @param output The PIDOutput object that is set to the output value
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, double Kff,
                INode input, PIDOutput output) {
    this(Kp, Ki, Kd, Kff, input, output, INode.DEFAULT_PERIOD);
  }

  /**
   * Free the PID object.
   */
  public void free() {
    m_output.disable();
  }

  /**
   * Set the PID Controller gain parameters.
   *
   * <p>Set the proportional, integral, and differential coefficients.
   *
   * @param Kp Proportional coefficient
   * @param Ki Integral coefficient
   * @param Kd Differential coefficient
   */
  @Override
  @SuppressWarnings("ParameterName")
  public void setPID(double Kp, double Ki, double Kd) {
    m_pid.setPID(Kp, Ki, Kd);
  }

  /**
   * Set the PID Controller gain parameters.
   *
   * <p>Set the proportional, integral, and differential coefficients.
   *
   * @param Kp  Proportional coefficient
   * @param Ki  Integral coefficient
   * @param Kd  Differential coefficient
   * @param Kff Feedforward coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setPID(double Kp, double Ki, double Kd, double Kff) {
    m_pid.setPID(Kp, Ki, Kd);
    m_feedforward.setGain(Kff);
  }

  /**
   * Get the Proportional coefficient.
   *
   * @return proportional coefficient
   */
  @Override
  public double getP() {
    return m_pid.getP();
  }

  /**
   * Get the Integral coefficient.
   *
   * @return integral coefficient
   */
  @Override
  public double getI() {
    return m_pid.getI();
  }

  /**
   * Get the Differential coefficient.
   *
   * @return differential coefficient
   */
  @Override
  public double getD() {
    return m_pid.getD();
  }

  /**
   * Get the Feedforward coefficient.
   *
   * @return feedforward coefficient
   */
  public double getF() {
    return m_feedforward.getGain();
  }

  /**
   * Set the PID controller to consider the input to be continuous.
   *
   * <p>Rather then using the max and min in as constraints, it considers them to be the same point
   * and automatically calculates the shortest route to the reference (e.g., gyroscope angle).
   *
   * @param continuous true turns on continuous; false turns off continuous
   */
  public void setContinuous(boolean continuous) {
    m_sum.setContinuous(continuous);
  }

  /**
   * Set the PID controller to consider the input to be continuous.
   *
   * <p>Rather then using the max and min in as constraints, it considers them to be the same point
   * and automatically calculates the shortest route to the reference (e.g., gyroscope angle).
   */
  public void setContinuous() {
    setContinuous(true);
  }

  /**
   * Sets the maximum and minimum values expected from the input.
   *
   * @param minimumInput the minimum value expected from the input
   * @param maximumInput the maximum value expected from the input
   */
  public void setInputRange(double minimumInput, double maximumInput) {
    m_sum.setInputRange(minimumInput, maximumInput);
  }

  /**
   * Sets the minimum and maximum values to write.
   *
   * @param minimumOutput the minimum value to write to the output
   * @param maximumOutput the maximum value to write to the output
   */
  public void setOutputRange(double minimumOutput, double maximumOutput) {
    m_output.setRange(minimumOutput, maximumOutput);
  }

  /**
   * Set maximum magnitude of error for which integration should occur. Values above this will
   * reset the current total.
   *
   * @param maxErrorMagnitude max value of error for which integration should occur
   */
  public void setIZone(double maxErrorMagnitude) {
    m_pid.setIZone(maxErrorMagnitude);
  }

  /**
   * Set the setpoint for the PIDController.
   *
   * @param setpoint the desired setpoint
   */
  @Override
  public void setSetpoint(double setpoint) {
    m_refInput.set(setpoint);
  }

  /**
   * Returns the current setpoint of the PIDController.
   *
   * @return the current setpoint
   */
  @Override
  public double getSetpoint() {
    return m_refInput.getOutput();
  }

  /**
   * Set the absolute error which is considered tolerable for use with OnTarget.
   *
   * @param tolerance absolute error which is tolerable
   * @param deltaTolerance change in absolute error which is tolerable
   */
  public void setAbsoluteTolerance(double tolerance, double deltaTolerance) {
    m_sum.setTolerance(tolerance, deltaTolerance);
  }

  /**
   * Set the absolute error which is considered tolerable for use with OnTarget.
   *
   * @param tolerance absolute error which is tolerable
   */
  public void setAbsoluteTolerance(double tolerance) {
    setAbsoluteTolerance(tolerance, Double.POSITIVE_INFINITY);
  }

  /**
   * Returns the current difference of the input from the setpoint.
   *
   * @return the current error
   */
  public double getError() {
    return m_sum.getOutput();
  }

  /**
   * Return true if the error and change in error is within the range determined by SetTolerance.
   */
  public boolean onTarget() {
    return m_sum.inTolerance();
  }

  /**
   * Begin running the PIDController.
   */
  @Override
  public void enable() {
    m_enabled = true;
    m_output.enable();
  }

  /**
   * Stop running the PIDController, this sets the output to zero before stopping.
   */
  @Override
  public void disable() {
    m_output.disable();
    m_enabled = false;
  }

  /**
   * Set the enabled state of the PIDController.
   */
  public void setEnabled(boolean enable) {
    if (enable) {
      enable();
    } else {
      disable();
    }
  }

  @Override
  public boolean isEnabled() {
    return m_enabled;
  }

  @Override
  public void reset() {
    disable();

    m_pid.reset();
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("PIDController");
    builder.setSafeState(this::reset);
    builder.addDoubleProperty("p", this::getP, m_pid::setP);
    builder.addDoubleProperty("i", this::getI, m_pid::setI);
    builder.addDoubleProperty("d", this::getD, m_pid::setD);
    builder.addDoubleProperty("f", this::getF, m_feedforward::setGain);
    builder.addDoubleProperty("setpoint", this::getSetpoint, this::setSetpoint);
    builder.addBooleanProperty("enabled", this::isEnabled, this::setEnabled);
  }
}
