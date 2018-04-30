/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

import edu.wpi.first.hal.sim.mockdata.SPIAccelerometerDataJNI;

public class SPIAccelerometerSim {
  private int m_index;

  public SPIAccelerometerSim(int index) {
    m_index = index;
  }

  public CallbackStore registerActiveCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = SPIAccelerometerDataJNI.registerActiveCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, SPIAccelerometerDataJNI::cancelActiveCallback);
  }
  public boolean getActive() {
    return SPIAccelerometerDataJNI.getActive(m_index);
  }
  public void setActive(boolean active) {
    SPIAccelerometerDataJNI.setActive(m_index, active);
  }

  public CallbackStore registerRangeCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = SPIAccelerometerDataJNI.registerRangeCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, SPIAccelerometerDataJNI::cancelRangeCallback);
  }
  public int getRange() {
    return SPIAccelerometerDataJNI.getRange(m_index);
  }
  public void setRange(int range) {
    SPIAccelerometerDataJNI.setRange(m_index, range);
  }

  public CallbackStore registerXCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = SPIAccelerometerDataJNI.registerXCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, SPIAccelerometerDataJNI::cancelXCallback);
  }
  public double getX() {
    return SPIAccelerometerDataJNI.getX(m_index);
  }
  public void setX(double x) {
    SPIAccelerometerDataJNI.setX(m_index, x);
  }

  public CallbackStore registerYCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = SPIAccelerometerDataJNI.registerYCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, SPIAccelerometerDataJNI::cancelYCallback);
  }
  public double getY() {
    return SPIAccelerometerDataJNI.getY(m_index);
  }
  public void setY(double y) {
    SPIAccelerometerDataJNI.setY(m_index, y);
  }

  public CallbackStore registerZCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = SPIAccelerometerDataJNI.registerZCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, SPIAccelerometerDataJNI::cancelZCallback);
  }
  public double getZ() {
    return SPIAccelerometerDataJNI.getZ(m_index);
  }
  public void setZ(double z) {
    SPIAccelerometerDataJNI.setZ(m_index, z);
  }

  public void resetData() {
    SPIAccelerometerDataJNI.resetData(m_index);
  }
}
