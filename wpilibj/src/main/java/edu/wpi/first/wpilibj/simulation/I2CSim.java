// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.BufferCallback;
import edu.wpi.first.hal.simulation.ConstBufferCallback;
import edu.wpi.first.hal.simulation.I2CDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;

public class I2CSim {
  private final int m_index;

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
