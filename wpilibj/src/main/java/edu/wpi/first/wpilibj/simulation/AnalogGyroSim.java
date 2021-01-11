// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.AnalogGyroDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.AnalogGyro;

/** Class to control a simulated analog gyro. */
public class AnalogGyroSim {
  private final int m_index;

  /**
   * Constructs from an AnalogGyro object.
   *
   * @param gyro AnalogGyro to simulate
   */
  public AnalogGyroSim(AnalogGyro gyro) {
    m_index = gyro.getAnalogInput().getChannel();
  }

  /**
   * Constructs from an analog input channel number.
   *
   * @param channel Channel number
   */
  public AnalogGyroSim(int channel) {
    m_index = channel;
  }

  /**
   * Register a callback on the angle.
   *
   * @param callback the callback that will be called whenever the angle changes
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerAngleCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogGyroDataJNI.registerAngleCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogGyroDataJNI::cancelAngleCallback);
  }

  /**
   * Get the current angle of the gyro.
   *
   * @return the angle measured by the gyro
   */
  public double getAngle() {
    return AnalogGyroDataJNI.getAngle(m_index);
  }

  /**
   * Change the angle measured by the gyro.
   *
   * @param angle the new value
   */
  public void setAngle(double angle) {
    AnalogGyroDataJNI.setAngle(m_index, angle);
  }

  /**
   * Register a callback on the rate.
   *
   * @param callback the callback that will be called whenever the rate changes
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerRateCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogGyroDataJNI.registerRateCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogGyroDataJNI::cancelRateCallback);
  }

  /**
   * Get the rate of angle change on this gyro.
   *
   * @return the rate
   */
  public double getRate() {
    return AnalogGyroDataJNI.getRate(m_index);
  }

  /**
   * Change the rate of the gyro.
   *
   * @param rate the new rate
   */
  public void setRate(double rate) {
    AnalogGyroDataJNI.setRate(m_index, rate);
  }

  /**
   * Register a callback on whether the gyro is initialized.
   *
   * @param callback the callback that will be called whenever the gyro is initialized
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogGyroDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogGyroDataJNI::cancelInitializedCallback);
  }

  /**
   * Check if the gyro is initialized.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return AnalogGyroDataJNI.getInitialized(m_index);
  }

  /**
   * Set whether this gyro is initialized.
   *
   * @param initialized the new value
   */
  public void setInitialized(boolean initialized) {
    AnalogGyroDataJNI.setInitialized(m_index, initialized);
  }

  /** Reset all simulation data for this object. */
  public void resetData() {
    AnalogGyroDataJNI.resetData(m_index);
  }
}
