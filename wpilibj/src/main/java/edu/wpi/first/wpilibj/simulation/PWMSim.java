// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.hal.simulation.PWMDataJNI;
import edu.wpi.first.wpilibj.PWM;
import edu.wpi.first.wpilibj.motorcontrol.PWMMotorController;

/** Class to control a simulated PWM output. */
public class PWMSim {
  private final int m_index;

  /**
   * Constructs from a PWM object.
   *
   * @param pwm PWM to simulate
   */
  public PWMSim(PWM pwm) {
    m_index = pwm.getChannel();
  }

  /**
   * Constructs from a PWMMotorController object.
   *
   * @param motorctrl PWMMotorController to simulate
   */
  public PWMSim(PWMMotorController motorctrl) {
    m_index = motorctrl.getChannel();
  }

  /**
   * Constructs from a PWM channel number.
   *
   * @param channel Channel number
   */
  public PWMSim(int channel) {
    m_index = channel;
  }

  /**
   * Register a callback to be run when the PWM is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelInitializedCallback);
  }

  /**
   * Check whether the PWM has been initialized.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return PWMDataJNI.getInitialized(m_index);
  }

  /**
   * Define whether the PWM has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  public void setInitialized(boolean initialized) {
    PWMDataJNI.setInitialized(m_index, initialized);
  }

  /**
   * Register a callback to be run when the PWM raw value changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerPulseMicrosecondCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerPulseMicrosecondCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelPulseMicrosecondCallback);
  }

  /**
   * Get the PWM pulse microsecond value.
   *
   * @return the PWM pulse microsecond value
   */
  public int getPulseMicrosecond() {
    return PWMDataJNI.getPulseMicrosecond(m_index);
  }

  /**
   * Set the PWM pulse microsecond value.
   *
   * @param microsecondPulseTime the PWM pulse microsecond value
   */
  public void setPulseMicrosecond(int microsecondPulseTime) {
    PWMDataJNI.setPulseMicrosecond(m_index, microsecondPulseTime);
  }
  /**
   * Register a callback to be run when the PWM period scale changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerOutputPeriodCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerOutputPeriodCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelOutputPeriodCallback);
  }

  /**
   * Get the PWM period scale.
   *
   * @return the PWM period scale
   */
  public int getOutputPeriod() {
    return PWMDataJNI.getOutputPeriod(m_index);
  }

  /**
   * Set the PWM period scale.
   *
   * @param period the PWM period scale
   */
  public void setOutputPeriod(int period) {
    PWMDataJNI.setOutputPeriod(m_index, period);
  }

  /** Reset all simulation data. */
  public void resetData() {
    PWMDataJNI.resetData(m_index);
  }
}
