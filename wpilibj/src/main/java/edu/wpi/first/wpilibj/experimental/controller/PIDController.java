/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.experimental.controller;

import java.util.concurrent.locks.ReentrantLock;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * Class implements a PID Control Loop.
 *
 * <p>This feedback controller runs in discrete time, so time deltas are not used in the integral
 * and derivative calculations. Therefore, the sample rate affects the controller's behavior for a
 * given set of PID constants.
 */
@SuppressWarnings("PMD.TooManyFields")
public class PIDController extends Controller implements Sendable, AutoCloseable {
  private String m_name = "";
  private String m_subsystem = "Ungrouped";

  private static int instances;

  // Factor for "proportional" control
  @SuppressWarnings("MemberName")
  private double m_Kp;

  // Factor for "integral" control
  @SuppressWarnings("MemberName")
  private double m_Ki;

  // Factor for "derivative" control
  @SuppressWarnings("MemberName")
  private double m_Kd;

  // The current value of the process variable.
  private double m_measurement;

  // |maximum output|
  private double m_maximumOutput = 1.0;

  // |minimum output|
  private double m_minimumOutput = -1.0;

  // Maximum input - limit setpoint to this
  private double m_maximumInput;

  // Minimum input - limit setpoint to this
  private double m_minimumInput;

  // input range - difference between maximum and minimum
  private double m_inputRange;

  // Do the endpoints wrap around? eg. Absolute encoder
  private boolean m_continuous;

  // The prior error (used to compute velocity)
  private double m_prevError;

  // The sum of the errors for use in the integral calc
  private double m_totalError;

  enum Tolerance {
    kAbsolute, kPercent;
  }

  private Tolerance m_toleranceType = Tolerance.kAbsolute;

  // The percentage or absolute error that is considered at reference.
  private double m_tolerance = 0.05;
  private double m_deltaTolerance = Double.POSITIVE_INFINITY;

  private double m_reference;
  private double m_output;

  /**
   * Allocate a PID object with the given constants for Kp, Ki, and Kd and a default period of 20ms.
   *
   * @param Kp The proportional coefficient.
   * @param Ki The integral coefficient.
   * @param Kd The derivative coefficient.
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd) {
    this(Kp, Ki, Kd, 0.02);
  }

  /**
   * Allocate a PID object with the given constants for Kp, Ki, and Kd, and given period.
   *
   * @param Kp     The proportional coefficient.
   * @param Ki     The integral coefficient.
   * @param Kd     The derivative coefficient.
   * @param period The period between controller updates in seconds.
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, double period) {
    super(period);

    m_Kp = Kp;
    m_Ki = Ki;
    m_Kd = Kd;

    instances++;
    HAL.report(tResourceType.kResourceType_PIDController, instances);
    m_name = "PIDController[" + instances + "]";
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
  @SuppressWarnings("ParameterName")
  public void setPID(double Kp, double Ki, double Kd) {
    m_thisMutex.lock();
    try {
      m_Kp = Kp;
      m_Ki = Ki;
      m_Kd = Kd;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the Proportional coefficient of the PID controller gain.
   *
   * @param Kp proportional coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setP(double Kp) {
    m_thisMutex.lock();
    try {
      m_Kp = Kp;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the Integral coefficient of the PID controller gain.
   *
   * @param Ki integral coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setI(double Ki) {
    m_thisMutex.lock();
    try {
      m_Ki = Ki;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the Differential coefficient of the PID controller gain.
   *
   * @param Kd differential coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setD(double Kd) {
    m_thisMutex.lock();
    try {
      m_Kd = Kd;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Get the Proportional coefficient.
   *
   * @return proportional coefficient
   */
  public double getP() {
    m_thisMutex.lock();
    try {
      return m_Kp;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Get the Integral coefficient.
   *
   * @return integral coefficient
   */
  public double getI() {
    m_thisMutex.lock();
    try {
      return m_Ki;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Get the Differential coefficient.
   *
   * @return differential coefficient
   */
  public double getD() {
    m_thisMutex.lock();
    try {
      return m_Kd;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Return the current controller output.
   *
   * <p>This is always centered around zero and constrained to the min and max outputs.
   *
   * @return The latest calculated output.
   */
  public double getOutput() {
    m_thisMutex.lock();
    try {
      return m_output;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Updates the controller with the latest measurement of the process variable.
   *
   * @param measurement The latest measurement of the process variable.
   */
  public void setMeasurement(double measurement) {
    m_thisMutex.lock();
    try {
      m_measurement = measurement;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the reference for the PIDController.
   *
   * @param reference the desired reference
   */
  public void setReference(double reference) {
    m_thisMutex.lock();
    try {
      if (m_maximumInput > m_minimumInput) {
        m_reference = clamp(reference, m_minimumInput, m_maximumInput);
      } else {
        m_reference = reference;
      }
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Returns the current reference of the PIDController.
   *
   * @return the current reference
   */
  public double getReference() {
    m_thisMutex.lock();
    try {
      return m_reference;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Return true if the error is within the percentage of the total input range,
   * determined by SetTolerance. This asssumes that the maximum and minimum
   * input were set using SetInput.
   *
   * <p>Currently this just reports on target as the actual value passes through the setpoint.
   * Ideally it should be based on being within the tolerance for some period of time.
   *
   * <p>This will return false until at least one input value has been computed.
   *
   * @return Whether the error is within the acceptable bounds.
   */
  public boolean atReference() {
    return atReference(m_tolerance, m_deltaTolerance, m_toleranceType);
  }

  /**
   * Return true if the error and change in error are below the specified tolerances.
   *
   * @param tolerance The maximum allowable error.
   * @param deltaTolerance The maximum allowable change in error from the previous iteration.
   * @param toleranceType Whether the given tolerance values are absolute, or percentages of the
   *                      total input range.
   * @return Whether the error is within the acceptable bounds.
   */
  public boolean atReference(double tolerance, double deltaTolerance, Tolerance toleranceType) {
    double error = getError();

    m_thisMutex.lock();
    try {
      double deltaError = (error - m_prevError) / getPeriod();
      if (toleranceType == Tolerance.kPercent) {
        return Math.abs(error) < tolerance / 100 * m_inputRange
            && Math.abs(deltaError) < deltaTolerance / 100 * m_inputRange;
      } else {
        return Math.abs(error) < tolerance
            && Math.abs(deltaError) < deltaTolerance;
      }
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the PID controller to consider the input to be continuous,
   *
   * <p>Rather then using the max and min input range as constraints, it considers them to be the
   * same point and automatically calculates the shortest route to the setpoint.
   */
  public void setContinuous() {
    setContinuous(true);
  }

  /**
   * Set the PID controller to consider the input to be continuous,
   *
   * <p>Rather then using the max and min input range as constraints, it considers them to be the
   * same point and automatically calculates the shortest route to the setpoint.
   *
   * @param continuous true turns on continuous, false turns off continuous
   */
  public void setContinuous(boolean continuous) {
    m_thisMutex.lock();
    try {
      m_continuous = continuous;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Sets the maximum and minimum values expected from the input.
   *
   * @param minimumInput the minimum value expected from the input
   * @param maximumInput the maximum value expected from the output
   */
  public void setInputRange(double minimumInput, double maximumInput) {
    m_thisMutex.lock();
    try {
      m_minimumInput = minimumInput;
      m_maximumInput = maximumInput;
      m_inputRange = maximumInput - minimumInput;
    } finally {
      m_thisMutex.unlock();
    }

    setReference(getReference());
  }

  /**
   * Sets the minimum and maximum values to write.
   *
   * @param minimumOutput the minimum value to write to the output
   * @param maximumOutput the maximum value to write to the output
   */
  public void setOutputRange(double minimumOutput, double maximumOutput) {
    m_thisMutex.lock();
    try {
      m_minimumOutput = minimumOutput;
      m_maximumOutput = maximumOutput;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the absolute error which is considered tolerable for use with atReference().
   *
   * @param tolerance Absolute error which is tolerable.
   */
  public void setAbsoluteTolerance(double tolerance) {
    setAbsoluteTolerance(tolerance, Double.POSITIVE_INFINITY);
  }

  /**
   * Set the absolute error which is considered tolerable for use with atReference().
   *
   * @param tolerance      Absolute error which is tolerable.
   * @param deltaTolerance Change in absolute error per second which is tolerable.
   */
  public void setAbsoluteTolerance(double tolerance, double deltaTolerance) {
    m_thisMutex.lock();
    try {
      m_toleranceType = Tolerance.kAbsolute;
      m_tolerance = tolerance;
      m_deltaTolerance = deltaTolerance;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the percent error which is considered tolerable for use with atReference().
   *
   * @param tolerance Percent error which is tolerable.
   */
  public void setPercentTolerance(double tolerance) {
    setPercentTolerance(tolerance, Double.POSITIVE_INFINITY);
  }

  /**
   * Set the percent error which is considered tolerable for use with atReference().
   *
   * @param tolerance      Percent error which is tolerable.
   * @param deltaTolerance Change in percent error per second which is tolerable.
   */
  public void setPercentTolerance(double tolerance, double deltaTolerance) {
    m_thisMutex.lock();
    try {
      m_toleranceType = Tolerance.kPercent;
      m_tolerance = tolerance;
      m_deltaTolerance = deltaTolerance;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Returns the difference between the reference and the measurement.
   *
   * @return The error.
   */
  public double getError() {
    m_thisMutex.lock();
    try {
      return getContinuousError(m_reference - m_measurement);
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Returns the change in error per second.
   */
  public double getDeltaError() {
    double error = getError();

    m_thisMutex.lock();
    try {
      return (error - m_prevError) / getPeriod();
    } finally {
      m_thisMutex.unlock();
    }
  }

  @Override
  @SuppressWarnings("LocalVariableName")
  public double calculate() {
    // Storage for function inputs
    final double Kp;
    final double Ki;
    final double Kd;
    final double measurement = m_measurement;
    final double minimumOutput;
    final double maximumOutput;

    // Storage for function input-outputs
    final double prevError;
    final double error;
    double totalError;

    m_thisMutex.lock();
    try {
      Kp = m_Kp;
      Ki = m_Ki;
      Kd = m_Kd;
      minimumOutput = m_minimumOutput;
      maximumOutput = m_maximumOutput;

      prevError = m_prevError;
      error = getContinuousError(m_reference - measurement);
      totalError = m_totalError;
    } finally {
      m_thisMutex.unlock();
    }

    if (Ki != 0) {
      totalError = clamp(totalError + error * getPeriod(), minimumOutput / Ki,
          maximumOutput / Ki);
    }

    double output =
        clamp(Kp * error + Ki * totalError + Kd * (error - prevError) / getPeriod(),
            minimumOutput, maximumOutput);

    m_thisMutex.lock();
    try {
      m_prevError = error;
      m_totalError = totalError;
      m_output = output;
    } finally {
      m_thisMutex.unlock();
    }

    return output;
  }

  @Override
  public double calculate(double reference, double measurement) {
    setReference(reference);
    setMeasurement(measurement);
    return calculate();
  }

  /**
   * Reset the previous error, the integral term, and disable the controller.
   */
  public void reset() {
    m_thisMutex.lock();
    try {
      m_prevError = 0;
      m_totalError = 0;
      m_output = 0;
    } finally {
      m_thisMutex.unlock();
    }
  }

  @Override
  public final synchronized String getName() {
    return m_name;
  }

  @Override
  public final synchronized void setName(String name) {
    m_name = name;
  }

  @Override
  public final synchronized String getSubsystem() {
    return m_subsystem;
  }

  @Override
  public final synchronized void setSubsystem(String subsystem) {
    m_subsystem = subsystem;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("PIDController");
    builder.setSafeState(this::reset);
    builder.addDoubleProperty("Kp", this::getP, this::setP);
    builder.addDoubleProperty("Ki", this::getI, this::setI);
    builder.addDoubleProperty("Kd", this::getD, this::setD);
    builder.addDoubleProperty("reference", this::getReference, this::setReference);
  }

  @Override
  public void close() {
    LiveWindow.remove(this);
  }

  protected ReentrantLock m_thisMutex = new ReentrantLock();

  /**
   * Wraps error around for continuous inputs. The original error is returned if
   * continuous mode is disabled. This is an unsynchronized function.
   *
   * @param error The current error of the PID controller.
   * @return Error for continuous inputs.
   */
  protected double getContinuousError(double error) {
    if (m_continuous && m_inputRange > 0) {
      error %= m_inputRange;
      if (Math.abs(error) > m_inputRange / 2) {
        if (error > 0) {
          return error - m_inputRange;
        } else {
          return error + m_inputRange;
        }
      }
    }

    return error;
  }

  private static double clamp(double value, double low, double high) {
    return Math.max(low, Math.min(value, high));
  }
}
