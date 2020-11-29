/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.AccelerometerDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.BuiltInAccelerometer;

/**
 * Class to control a simulated built-in accelerometer.
 */
public class BuiltInAccelerometerSim {
  private final int m_index;

  /**
   * Constructs for the first built-in accelerometer.
   */
  public BuiltInAccelerometerSim() {
    m_index = 0;
  }

  /**
   * Constructs from a BuiltInAccelerometer object.
   *
   * @param accel BuiltInAccelerometer to simulate
   */
  @SuppressWarnings("PMD.UnusedFormalParameter")
  public BuiltInAccelerometerSim(BuiltInAccelerometer accel) {
    m_index = 0;
  }

  public void setDisplayName(String displayName) {
    AccelerometerDataJNI.setDisplayName(m_index, displayName);
  }

  public CallbackStore registerActiveCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AccelerometerDataJNI.registerActiveCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AccelerometerDataJNI::cancelActiveCallback);
  }
  public boolean getActive() {
    return AccelerometerDataJNI.getActive(m_index);
  }
  public void setActive(boolean active) {
    AccelerometerDataJNI.setActive(m_index, active);
  }

  public CallbackStore registerRangeCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AccelerometerDataJNI.registerRangeCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AccelerometerDataJNI::cancelRangeCallback);
  }
  public int getRange() {
    return AccelerometerDataJNI.getRange(m_index);
  }
  public void setRange(int range) {
    AccelerometerDataJNI.setRange(m_index, range);
  }

  public CallbackStore registerXCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AccelerometerDataJNI.registerXCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AccelerometerDataJNI::cancelXCallback);
  }
  public double getX() {
    return AccelerometerDataJNI.getX(m_index);
  }
  public void setX(double x) {
    AccelerometerDataJNI.setX(m_index, x);
  }

  public CallbackStore registerYCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AccelerometerDataJNI.registerYCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AccelerometerDataJNI::cancelYCallback);
  }
  public double getY() {
    return AccelerometerDataJNI.getY(m_index);
  }
  public void setY(double y) {
    AccelerometerDataJNI.setY(m_index, y);
  }

  public CallbackStore registerZCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AccelerometerDataJNI.registerZCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AccelerometerDataJNI::cancelZCallback);
  }
  public double getZ() {
    return AccelerometerDataJNI.getZ(m_index);
  }
  public void setZ(double z) {
    AccelerometerDataJNI.setZ(m_index, z);
  }

  public void resetData() {
    AccelerometerDataJNI.resetData(m_index);
  }
}
