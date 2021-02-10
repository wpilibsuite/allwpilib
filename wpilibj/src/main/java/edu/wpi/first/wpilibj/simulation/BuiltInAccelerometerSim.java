// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.AccelerometerDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.BuiltInAccelerometer;

/** Class to control a simulated built-in accelerometer. */
public class BuiltInAccelerometerSim {
  private final int m_index;

  /** Constructs for the first built-in accelerometer. */
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

  /**
   * Register a callback to be run when this accelerometer activates.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerActiveCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AccelerometerDataJNI.registerActiveCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AccelerometerDataJNI::cancelActiveCallback);
  }

  /**
   * Check whether the accelerometer is active.
   *
   * @return true if active
   */
  public boolean getActive() {
    return AccelerometerDataJNI.getActive(m_index);
  }

  /**
   * Define whether this accelerometer is active.
   *
   * @param active the new state
   */
  public void setActive(boolean active) {
    AccelerometerDataJNI.setActive(m_index, active);
  }

  /**
   * Register a callback to be run whenever the range changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerRangeCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AccelerometerDataJNI.registerRangeCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AccelerometerDataJNI::cancelRangeCallback);
  }

  /**
   * Check the range of this accelerometer.
   *
   * @return the accelerometer range
   */
  public int getRange() {
    return AccelerometerDataJNI.getRange(m_index);
  }

  /**
   * Change the range of this accelerometer.
   *
   * @param range the new accelerometer range
   */
  public void setRange(int range) {
    AccelerometerDataJNI.setRange(m_index, range);
  }

  /**
   * Register a callback to be run whenever the X axis value changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerXCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AccelerometerDataJNI.registerXCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AccelerometerDataJNI::cancelXCallback);
  }

  /**
   * Measure the X axis value.
   *
   * @return the X axis measurement
   */
  public double getX() {
    return AccelerometerDataJNI.getX(m_index);
  }

  /**
   * Change the X axis value of the accelerometer.
   *
   * @param x the new reading of the X axis
   */
  @SuppressWarnings("ParameterName")
  public void setX(double x) {
    AccelerometerDataJNI.setX(m_index, x);
  }

  /**
   * Register a callback to be run whenever the Y axis value changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerYCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AccelerometerDataJNI.registerYCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AccelerometerDataJNI::cancelYCallback);
  }

  /**
   * Measure the Y axis value.
   *
   * @return the Y axis measurement
   */
  public double getY() {
    return AccelerometerDataJNI.getY(m_index);
  }

  /**
   * Change the Y axis value of the accelerometer.
   *
   * @param y the new reading of the Y axis
   */
  @SuppressWarnings("ParameterName")
  public void setY(double y) {
    AccelerometerDataJNI.setY(m_index, y);
  }

  /**
   * Register a callback to be run whenever the Z axis value changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerZCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AccelerometerDataJNI.registerZCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AccelerometerDataJNI::cancelZCallback);
  }

  /**
   * Measure the Z axis value.
   *
   * @return the Z axis measurement
   */
  public double getZ() {
    return AccelerometerDataJNI.getZ(m_index);
  }

  /**
   * Change the Z axis value of the accelerometer.
   *
   * @param z the new reading of the Z axis
   */
  @SuppressWarnings("ParameterName")
  public void setZ(double z) {
    AccelerometerDataJNI.setZ(m_index, z);
  }

  /** Reset all simulation data of this object. */
  public void resetData() {
    AccelerometerDataJNI.resetData(m_index);
  }
}
