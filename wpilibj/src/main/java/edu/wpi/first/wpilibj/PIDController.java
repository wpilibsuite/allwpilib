/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

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
public class PIDController extends PIDBase implements Controller {
  private Notifier m_controlLoop = new Notifier(this::calculate);
  private double m_period;

  /**
   * Allocate a PID object with the given constants for Kp, Ki, Kd, and Kv.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param Kv     the velocity feedforward term
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output percentage
   * @param period the loop time for doing calculations. This particularly effects calculations of
   *               the integral and differential terms. The default is 50ms.
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, double Kv, PIDSource source,
                       PIDOutput output, double period) {
    this(Kp, Ki, Kd, Kv, 0.0, source, output, period);
  }

  /**
   * Allocate a PID object with the given constants for Kp, Ki, Kd, Kv, and Ka.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param Kv     the velocity feedforward term
   * @param Ka     the acceleration feedforward term
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output percentage
   * @param period the loop time for doing calculations. This particularly effects calculations of
   *               the integral and differential terms. The default is 50ms.
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, double Kv, double Ka, PIDSource source,
                       PIDOutput output, double period) {
    super(Kp, Ki, Kd, Kv, Ka, source, output);
    m_controlLoop.startPeriodic(period);
  }

  /**
   * Allocate a PID object with the given constants for Kp, Ki, Kd, and period.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param source the PIDSource object that is used to get values
   * @param output the PIDOutput object that is set to the output percentage
   * @param period the loop time for doing calculations. This particularly effects calculations of
   *               the integral and differential terms. The default is 50ms.
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, PIDSource source, PIDOutput output,
                       double period) {
    this(Kp, Ki, Kd, 0.0, 0.0, source, output, period);
  }

  /**
   * Allocate a PID object with the given constants for Kp, Ki, and Kd using a 50ms period.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output percentage
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, PIDSource source, PIDOutput output) {
    this(Kp, Ki, Kd, source, output, kDefaultPeriod);
  }

  /**
   * Allocate a PID object with the given constants for Kp, Ki, Kd, and Kv using a 50ms period.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param Kv     the velocity feedforward term
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output percentage
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, double Kv, PIDSource source,
                       PIDOutput output) {
    this(Kp, Ki, Kd, Kv, source, output, kDefaultPeriod);
  }

  /**
   * Allocate a PID object with the given constants for Kp, Ki, Ki, Kv, and Ka using a 50ms period.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param Kv     the velocity feedforward term
   * @param Ka     the acceleration feedforward term
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output percentage
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, double Kv, double Ka, PIDSource source,
                       PIDOutput output) {
    this(Kp, Ki, Kd, Kv, Ka, source, output, kDefaultPeriod);
  }

  @Override
  public void close() {
    super.close();
    m_controlLoop.close();
    m_thisMutex.lock();
    try {
      m_pidOutput = null;
      m_pidInput = null;
      m_controlLoop = null;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Begin running the PIDController.
   */
  @Override
  public void enable() {
    m_thisMutex.lock();
    try {
      m_enabled = true;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Stop running the PIDController, this sets the output to zero before stopping.
   */
  @Override
  public void disable() {
    // Ensures m_enabled check and pidWrite() call occur atomically
    m_pidWriteMutex.lock();
    try {
      m_thisMutex.lock();
      try {
        m_enabled = false;
      } finally {
        m_thisMutex.unlock();
      }

      m_pidOutput.pidWrite(0);
    } finally {
      m_pidWriteMutex.unlock();
    }
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

  /**
   * Return true if PIDController is enabled.
   */
  public boolean isEnabled() {
    m_thisMutex.lock();
    try {
      return m_enabled;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Calculate the feed forward term.
   *
   * <p>Both of the provided feed forward calculations are velocity feed forwards. If a different
   * feed forward calculation is desired, the user can override this function and provide his or
   * her own. This function  does no synchronization because the PIDController class only calls it
   * in synchronized code, so be careful if calling it oneself.
   *
   * <p>If a velocity PID controller is being used, the Kv term should be set to 1 over the maximum
   * setpoint for the output. If a position PID controller is being used, the Kv term should be set
   * to 1 over the maximum speed for the output measured in setpoint units per second.
   */
  @Override
  protected double calculateFeedForward() {
    if (m_pidInput.getPIDSourceType().equals(PIDSourceType.kRate)) {
      return (getV() * getSetpoint() + getA() * getDeltaSetpoint()) * m_period;
    } else {
      final double deltaSetpoint = getDeltaSetpoint();
      final double output = getV() * deltaSetpoint + getA() * (deltaSetpoint - m_prevDeltaSetpoint);
      m_prevSetpoint = getSetpoint();
      m_prevDeltaSetpoint = deltaSetpoint;
      m_setpointTimer.reset();
      return output * m_period;
    }
  }

  /**
   * Reset the previous error, the integral term, and disable the controller.
   */
  @Override
  public void reset() {
    disable();

    super.reset();
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    super.initSendable(builder);
    builder.addBooleanProperty("enabled", this::isEnabled, this::setEnabled);
  }
}
