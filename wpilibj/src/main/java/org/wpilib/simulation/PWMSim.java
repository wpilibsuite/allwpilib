// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import org.wpilib.hardware.discrete.PWMOutput;
import org.wpilib.hardware.hal.simulation.NotifyCallback;
import org.wpilib.hardware.hal.simulation.PWMDataJNI;

/** Class to control a simulated PWMOutput output. */
public class PWMSim {
  private final int m_index;

  /**
   * Constructs from a PWMOutput object.
   *
   * @param pwm PWMOutput to simulate
   */
  public PWMSim(PWMOutput pwm) {
    m_index = pwm.getChannel();
  }

  /**
   * Constructs from a PWMOutput channel number.
   *
   * @param channel Channel number
   */
  public PWMSim(int channel) {
    m_index = channel;
  }

  /**
   * Register a callback to be run when the PWMOutput is initialized.
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
   * Check whether the PWMOutput has been initialized.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return PWMDataJNI.getInitialized(m_index);
  }

  /**
   * Define whether the PWMOutput has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  public void setInitialized(boolean initialized) {
    PWMDataJNI.setInitialized(m_index, initialized);
  }

  /**
   * Register a callback to be run when the PWMOutput raw value changes.
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
   * Get the PWMOutput pulse microsecond value.
   *
   * @return the PWMOutput pulse microsecond value
   */
  public int getPulseMicrosecond() {
    return PWMDataJNI.getPulseMicrosecond(m_index);
  }

  /**
   * Set the PWMOutput pulse microsecond value.
   *
   * @param microsecondPulseTime the PWMOutput pulse microsecond value
   */
  public void setPulseMicrosecond(int microsecondPulseTime) {
    PWMDataJNI.setPulseMicrosecond(m_index, microsecondPulseTime);
  }

  /**
   * Register a callback to be run when the PWMOutput period scale changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerOutputPeriodCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerOutputPeriodCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelOutputPeriodCallback);
  }

  /**
   * Get the PWMOutput period scale.
   *
   * @return the PWMOutput period scale
   */
  public int getOutputPeriod() {
    return PWMDataJNI.getOutputPeriod(m_index);
  }

  /**
   * Set the PWMOutput period scale.
   *
   * @param period the PWMOutput period scale
   */
  public void setOutputPeriod(int period) {
    PWMDataJNI.setOutputPeriod(m_index, period);
  }

  /** Reset all simulation data. */
  public void resetData() {
    PWMDataJNI.resetData(m_index);
  }
}
