// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.BufferCallback;
import edu.wpi.first.hal.simulation.ConstBufferCallback;
import edu.wpi.first.hal.simulation.I2CDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;

/** A class to control a simulated I2C device. */
public class I2CSim {
  private final int m_index;

  /**
   * Construct a new I2C simulation object.
   *
   * @param index the HAL index of the I2C object
   */
  public I2CSim(int index) {
    m_index = index;
  }

  /**
   * Register a callback to be run when this I2C device is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = I2CDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, I2CDataJNI::cancelInitializedCallback);
  }

  /**
   * Check whether this I2C device has been initialized.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return I2CDataJNI.getInitialized(m_index);
  }

  /**
   * Define whether this I2C device has been initialized.
   *
   * @param initialized whether this device is initialized
   */
  public void setInitialized(boolean initialized) {
    I2CDataJNI.setInitialized(m_index, initialized);
  }

  /**
   * Register a callback to be run whenever a `read` operation is done.
   *
   * @param callback the callback that is run on `read` operations
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerReadCallback(BufferCallback callback) {
    int uid = I2CDataJNI.registerReadCallback(m_index, callback);
    return new CallbackStore(m_index, uid, I2CDataJNI::cancelReadCallback);
  }

  /**
   * Register a callback to be run whenever a `write` operation is done.
   *
   * @param callback the callback that is run on `write` operations
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerWriteCallback(ConstBufferCallback callback) {
    int uid = I2CDataJNI.registerWriteCallback(m_index, callback);
    return new CallbackStore(m_index, uid, I2CDataJNI::cancelWriteCallback);
  }

  /** Reset all I2C simulation data. */
  public void resetData() {
    I2CDataJNI.resetData(m_index);
  }
}
