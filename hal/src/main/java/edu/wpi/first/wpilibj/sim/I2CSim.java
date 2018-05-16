/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

import edu.wpi.first.hal.sim.mockdata.I2CDataJNI;

public class I2CSim {
  private int m_index;

  public I2CSim(int index) {
    m_index = index;
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = I2CDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, I2CDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return I2CDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    I2CDataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerReadCallback(BufferCallback callback) {
    int uid = I2CDataJNI.registerReadCallback(m_index, callback);
    return new CallbackStore(m_index, uid, I2CDataJNI::cancelReadCallback);
  }

  public CallbackStore registerWriteCallback(ConstBufferCallback callback) {
    int uid = I2CDataJNI.registerWriteCallback(m_index, callback);
    return new CallbackStore(m_index, uid, I2CDataJNI::cancelWriteCallback);
  }

  public void resetData() {
    I2CDataJNI.resetData(m_index);
  }
}
