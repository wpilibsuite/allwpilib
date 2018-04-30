/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

import edu.wpi.first.hal.sim.mockdata.SPIDataJNI;

public class SPISim {
  private int m_index;

  public SPISim(int index) {
    m_index = index;
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = SPIDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, SPIDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return SPIDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    SPIDataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerReadCallback(BufferCallback callback) {
    int uid = SPIDataJNI.registerReadCallback(m_index, callback);
    return new CallbackStore(m_index, uid, SPIDataJNI::cancelReadCallback);
  }

  public CallbackStore registerWriteCallback(ConstBufferCallback callback) {
    int uid = SPIDataJNI.registerWriteCallback(m_index, callback);
    return new CallbackStore(m_index, uid, SPIDataJNI::cancelWriteCallback);
  }

  public CallbackStore registerReadAutoReceiveBufferCallback(SpiReadAutoReceiveBufferCallback callback) {
    int uid = SPIDataJNI.registerReadAutoReceiveBufferCallback(m_index, callback);
    return new CallbackStore(m_index, uid, SPIDataJNI::cancelReadAutoReceiveBufferCallback);
  }

  public void resetData() {
    SPIDataJNI.resetData(m_index);
  }
}
