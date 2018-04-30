/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

import edu.wpi.first.hal.sim.mockdata.AnalogOutDataJNI;

public class AnalogOutSim {
  private int m_index;

  public AnalogOutSim(int index) {
    m_index = index;
  }

  public CallbackStore registerVoltageCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogOutDataJNI.registerVoltageCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogOutDataJNI::cancelVoltageCallback);
  }
  public double getVoltage() {
    return AnalogOutDataJNI.getVoltage(m_index);
  }
  public void setVoltage(double voltage) {
    AnalogOutDataJNI.setVoltage(m_index, voltage);
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogOutDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogOutDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return AnalogOutDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    AnalogOutDataJNI.setInitialized(m_index, initialized);
  }

  public void resetData() {
    AnalogOutDataJNI.resetData(m_index);
  }
}
