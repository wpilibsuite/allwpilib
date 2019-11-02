/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.sim;

import edu.wpi.first.hal.sim.mockdata.DutyCycleDataJNI;

public class DutyCycleSim {
  private final int m_index;

  public DutyCycleSim(int index) {
    m_index = index;
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DutyCycleDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DutyCycleDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return DutyCycleDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    DutyCycleDataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerFrequencyCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DutyCycleDataJNI.registerFrequencyCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DutyCycleDataJNI::cancelFrequencyCallback);
  }
  public int getFrequency() {
    return DutyCycleDataJNI.getFrequency(m_index);
  }
  public void setFrequency(int frequency) {
    DutyCycleDataJNI.setFrequency(m_index, frequency);
  }

  public CallbackStore registerOutputCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DutyCycleDataJNI.registerOutputCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DutyCycleDataJNI::cancelOutputCallback);
  }
  public double getOutput() {
    return DutyCycleDataJNI.getOutput(m_index);
  }
  public void setOutput(double output) {
    DutyCycleDataJNI.setOutput(m_index, output);
  }
}
