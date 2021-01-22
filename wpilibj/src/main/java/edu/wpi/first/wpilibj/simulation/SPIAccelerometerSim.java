// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.hal.simulation.SPIAccelerometerDataJNI;

/** A class to control a simulated accelerometer over SPI. */
public class SPIAccelerometerSim {
  private final int m_index;

  /**
   * Construct a new simulation object.
   *
   * @param index the HAL index of the accelerometer
   */
  public SPIAccelerometerSim(int index) {
    m_index = index;
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
    int uid = SPIAccelerometerDataJNI.registerActiveCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, SPIAccelerometerDataJNI::cancelActiveCallback);
  }

  /**
   * Check whether the accelerometer is active.
   *
   * @return true if active
   */
  public boolean getActive() {
    return SPIAccelerometerDataJNI.getActive(m_index);
  }

  /**
   * Define whether this accelerometer is active.
   *
   * @param active the new state
   */
  public void setActive(boolean active) {
    SPIAccelerometerDataJNI.setActive(m_index, active);
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
    int uid = SPIAccelerometerDataJNI.registerRangeCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, SPIAccelerometerDataJNI::cancelRangeCallback);
  }

  /**
   * Check the range of this accelerometer.
   *
   * @return the accelerometer range
   */
  public int getRange() {
    return SPIAccelerometerDataJNI.getRange(m_index);
  }

  /**
   * Change the range of this accelerometer.
   *
   * @param range the new accelerometer range
   */
  public void setRange(int range) {
    SPIAccelerometerDataJNI.setRange(m_index, range);
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
    int uid = SPIAccelerometerDataJNI.registerXCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, SPIAccelerometerDataJNI::cancelXCallback);
  }

  /**
   * Measure the X axis value.
   *
   * @return the X axis measurement
   */
  public double getX() {
    return SPIAccelerometerDataJNI.getX(m_index);
  }

  /**
   * Change the X axis value of the accelerometer.
   *
   * @param x the new reading of the X axis
   */
  @SuppressWarnings("ParameterName")
  public void setX(double x) {
    SPIAccelerometerDataJNI.setX(m_index, x);
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
    int uid = SPIAccelerometerDataJNI.registerYCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, SPIAccelerometerDataJNI::cancelYCallback);
  }

  /**
   * Measure the Y axis value.
   *
   * @return the Y axis measurement
   */
  public double getY() {
    return SPIAccelerometerDataJNI.getY(m_index);
  }

  /**
   * Change the Y axis value of the accelerometer.
   *
   * @param y the new reading of the Y axis
   */
  @SuppressWarnings("ParameterName")
  public void setY(double y) {
    SPIAccelerometerDataJNI.setY(m_index, y);
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
    int uid = SPIAccelerometerDataJNI.registerZCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, SPIAccelerometerDataJNI::cancelZCallback);
  }

  /**
   * Measure the Z axis value.
   *
   * @return the Z axis measurement
   */
  public double getZ() {
    return SPIAccelerometerDataJNI.getZ(m_index);
  }

  /**
   * Change the Z axis value of the accelerometer.
   *
   * @param z the new reading of the Z axis
   */
  @SuppressWarnings("ParameterName")
  public void setZ(double z) {
    SPIAccelerometerDataJNI.setZ(m_index, z);
  }

  /** Reset all simulation data of this object. */
  public void resetData() {
    SPIAccelerometerDataJNI.resetData(m_index);
  }
}
