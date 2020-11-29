/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.AnalogGyroDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.AnalogGyro;

/**
 * Class to control a simulated analog gyro.
 */
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

  public void setDisplayName(String displayName) {
    AnalogGyroDataJNI.setDisplayName(m_index, displayName);
  }

  /**
   * Constructs from an analog input channel number.
   *
   * @param channel Channel number
   */
  public AnalogGyroSim(int channel) {
    m_index = channel;
  }

  public CallbackStore registerAngleCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogGyroDataJNI.registerAngleCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogGyroDataJNI::cancelAngleCallback);
  }
  public double getAngle() {
    return AnalogGyroDataJNI.getAngle(m_index);
  }
  public void setAngle(double angle) {
    AnalogGyroDataJNI.setAngle(m_index, angle);
  }

  public CallbackStore registerRateCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogGyroDataJNI.registerRateCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogGyroDataJNI::cancelRateCallback);
  }
  public double getRate() {
    return AnalogGyroDataJNI.getRate(m_index);
  }
  public void setRate(double rate) {
    AnalogGyroDataJNI.setRate(m_index, rate);
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogGyroDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogGyroDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return AnalogGyroDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    AnalogGyroDataJNI.setInitialized(m_index, initialized);
  }

  public void resetData() {
    AnalogGyroDataJNI.resetData(m_index);
  }
}
