/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.hal.simulation.PWMDataJNI;
import edu.wpi.first.wpilibj.PWM;

/**
 * Class to control a simulated PWM output.
 */
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

  public String getDisplayName() {
    return PWMDataJNI.getDisplayName(m_index);
  }

  public void setDisplayName(String displayName) {
    PWMDataJNI.setDisplayName(m_index, displayName);
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return PWMDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    PWMDataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerRawValueCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerRawValueCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelRawValueCallback);
  }
  public int getRawValue() {
    return PWMDataJNI.getRawValue(m_index);
  }
  public void setRawValue(int rawValue) {
    PWMDataJNI.setRawValue(m_index, rawValue);
  }

  public CallbackStore registerSpeedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerSpeedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelSpeedCallback);
  }
  public double getSpeed() {
    return PWMDataJNI.getSpeed(m_index);
  }
  public void setSpeed(double speed) {
    PWMDataJNI.setSpeed(m_index, speed);
  }

  public CallbackStore registerPositionCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerPositionCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelPositionCallback);
  }
  public double getPosition() {
    return PWMDataJNI.getPosition(m_index);
  }
  public void setPosition(double position) {
    PWMDataJNI.setPosition(m_index, position);
  }

  public CallbackStore registerPeriodScaleCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerPeriodScaleCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelPeriodScaleCallback);
  }
  public int getPeriodScale() {
    return PWMDataJNI.getPeriodScale(m_index);
  }
  public void setPeriodScale(int periodScale) {
    PWMDataJNI.setPeriodScale(m_index, periodScale);
  }

  public CallbackStore registerZeroLatchCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PWMDataJNI.registerZeroLatchCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PWMDataJNI::cancelZeroLatchCallback);
  }
  public boolean getZeroLatch() {
    return PWMDataJNI.getZeroLatch(m_index);
  }
  public void setZeroLatch(boolean zeroLatch) {
    PWMDataJNI.setZeroLatch(m_index, zeroLatch);
  }

  public void resetData() {
    PWMDataJNI.resetData(m_index);
  }
}
