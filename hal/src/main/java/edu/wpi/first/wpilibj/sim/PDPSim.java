/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

import edu.wpi.first.hal.sim.mockdata.PDPDataJNI;

public class PDPSim {
  private int m_index;

  public PDPSim(int index) {
    m_index = index;
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PDPDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PDPDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return PDPDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    PDPDataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerTemperatureCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PDPDataJNI.registerTemperatureCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PDPDataJNI::cancelTemperatureCallback);
  }
  public double getTemperature() {
    return PDPDataJNI.getTemperature(m_index);
  }
  public void setTemperature(double temperature) {
    PDPDataJNI.setTemperature(m_index, temperature);
  }

  public CallbackStore registerVoltageCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PDPDataJNI.registerVoltageCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PDPDataJNI::cancelVoltageCallback);
  }
  public double getVoltage() {
    return PDPDataJNI.getVoltage(m_index);
  }
  public void setVoltage(double voltage) {
    PDPDataJNI.setVoltage(m_index, voltage);
  }

  public CallbackStore registerCurrentCallback(int channel, NotifyCallback callback, boolean initialNotify) {
    int uid = PDPDataJNI.registerCurrentCallback(m_index, channel, callback, initialNotify);
    return new CallbackStore(m_index, channel, uid, PDPDataJNI::cancelCurrentCallback);
  }
  public double getCurrent(int channel) {
    return PDPDataJNI.getCurrent(channel, m_index);
  }
  public void setCurrent(int channel, double current) {
    PDPDataJNI.setCurrent(m_index, channel, current);
  }

  public void resetData() {
    PDPDataJNI.resetData(m_index);
  }
}
