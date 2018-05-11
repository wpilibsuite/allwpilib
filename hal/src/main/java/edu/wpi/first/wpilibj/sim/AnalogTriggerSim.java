/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

import edu.wpi.first.hal.sim.mockdata.AnalogTriggerDataJNI;

public class AnalogTriggerSim {
  private int m_index;

  public AnalogTriggerSim(int index) {
    m_index = index;
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogTriggerDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogTriggerDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return AnalogTriggerDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    AnalogTriggerDataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerTriggerLowerBoundCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogTriggerDataJNI.registerTriggerLowerBoundCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogTriggerDataJNI::cancelTriggerLowerBoundCallback);
  }
  public double getTriggerLowerBound() {
    return AnalogTriggerDataJNI.getTriggerLowerBound(m_index);
  }
  public void setTriggerLowerBound(double triggerLowerBound) {
    AnalogTriggerDataJNI.setTriggerLowerBound(m_index, triggerLowerBound);
  }

  public CallbackStore registerTriggerUpperBoundCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogTriggerDataJNI.registerTriggerUpperBoundCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogTriggerDataJNI::cancelTriggerUpperBoundCallback);
  }
  public double getTriggerUpperBound() {
    return AnalogTriggerDataJNI.getTriggerUpperBound(m_index);
  }
  public void setTriggerUpperBound(double triggerUpperBound) {
    AnalogTriggerDataJNI.setTriggerUpperBound(m_index, triggerUpperBound);
  }

  public void resetData() {
    AnalogTriggerDataJNI.resetData(m_index);
  }
}
