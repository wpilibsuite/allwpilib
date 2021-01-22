// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.hal.simulation.PWMDataJNI;
import edu.wpi.first.wpilibj.PWM;

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
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
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
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerRawValueCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerRawValueCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelRawValueCallback);
  }

  /**
   * Get the PWM raw value.
   *
   * @return the PWM raw value
   */
  public int getRawValue() {
    return PWMDataJNI.getRawValue(m_index);
  }

  /**
   * Set the PWM raw value.
   *
   * @param rawValue the PWM raw value
   */
  public void setRawValue(int rawValue) {
    PWMDataJNI.setRawValue(m_index, rawValue);
  }

  /**
   * Register a callback to be run when the PWM speed changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerSpeedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerSpeedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelSpeedCallback);
  }

  /**
   * Get the PWM speed.
   *
   * @return the PWM speed (-1.0 to 1.0)
   */
  public double getSpeed() {
    return PWMDataJNI.getSpeed(m_index);
  }

  /**
   * Set the PWM speed.
   *
   * @param speed the PWM speed (-1.0 to 1.0)
   */
  public void setSpeed(double speed) {
    PWMDataJNI.setSpeed(m_index, speed);
  }

  /**
   * Register a callback to be run when the PWM position changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerPositionCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerPositionCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelPositionCallback);
  }

  /**
   * Get the PWM position.
   *
   * @return the PWM position (0.0 to 1.0)
   */
  public double getPosition() {
    return PWMDataJNI.getPosition(m_index);
  }

  /**
   * Set the PWM position.
   *
   * @param position the PWM position (0.0 to 1.0)
   */
  public void setPosition(double position) {
    PWMDataJNI.setPosition(m_index, position);
  }

  /**
   * Register a callback to be run when the PWM period scale changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerPeriodScaleCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerPeriodScaleCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelPeriodScaleCallback);
  }

  /**
   * Get the PWM period scale.
   *
   * @return the PWM period scale
   */
  public int getPeriodScale() {
    return PWMDataJNI.getPeriodScale(m_index);
  }

  /**
   * Set the PWM period scale.
   *
   * @param periodScale the PWM period scale
   */
  public void setPeriodScale(int periodScale) {
    PWMDataJNI.setPeriodScale(m_index, periodScale);
  }

  /**
   * Register a callback to be run when the PWM zero latch state changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerZeroLatchCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerZeroLatchCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelZeroLatchCallback);
  }

  /**
   * Check whether the PWM is zero latched.
   *
   * @return true if zero latched
   */
  public boolean getZeroLatch() {
    return PWMDataJNI.getZeroLatch(m_index);
  }

  /**
   * Define whether the PWM has been zero latched.
   *
   * @param zeroLatch true to indicate zero latched
   */
  public void setZeroLatch(boolean zeroLatch) {
    PWMDataJNI.setZeroLatch(m_index, zeroLatch);
  }

  /** Reset all simulation data. */
  public void resetData() {
    PWMDataJNI.resetData(m_index);
  }
}
