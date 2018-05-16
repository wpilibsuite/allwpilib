/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

import edu.wpi.first.hal.sim.mockdata.RelayDataJNI;

public class RelaySim {
  private int m_index;

  public RelaySim(int index) {
    m_index = index;
  }

  public CallbackStore registerInitializedForwardCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RelayDataJNI.registerInitializedForwardCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RelayDataJNI::cancelInitializedForwardCallback);
  }
  public boolean getInitializedForward() {
    return RelayDataJNI.getInitializedForward(m_index);
  }
  public void setInitializedForward(boolean initializedForward) {
    RelayDataJNI.setInitializedForward(m_index, initializedForward);
  }

  public CallbackStore registerInitializedReverseCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RelayDataJNI.registerInitializedReverseCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RelayDataJNI::cancelInitializedReverseCallback);
  }
  public boolean getInitializedReverse() {
    return RelayDataJNI.getInitializedReverse(m_index);
  }
  public void setInitializedReverse(boolean initializedReverse) {
    RelayDataJNI.setInitializedReverse(m_index, initializedReverse);
  }

  public CallbackStore registerForwardCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RelayDataJNI.registerForwardCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RelayDataJNI::cancelForwardCallback);
  }
  public boolean getForward() {
    return RelayDataJNI.getForward(m_index);
  }
  public void setForward(boolean forward) {
    RelayDataJNI.setForward(m_index, forward);
  }

  public CallbackStore registerReverseCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RelayDataJNI.registerReverseCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RelayDataJNI::cancelReverseCallback);
  }
  public boolean getReverse() {
    return RelayDataJNI.getReverse(m_index);
  }
  public void setReverse(boolean reverse) {
    RelayDataJNI.setReverse(m_index, reverse);
  }

  public void resetData() {
    RelayDataJNI.resetData(m_index);
  }
}
