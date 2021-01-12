// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.BufferCallback;
import edu.wpi.first.hal.simulation.ConstBufferCallback;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.hal.simulation.SPIDataJNI;
import edu.wpi.first.hal.simulation.SpiReadAutoReceiveBufferCallback;

/** A class for controlling a simulated SPI device. */
public class SPISim {
  private final int m_index;

  /** Create a new simulated SPI device. */
  public SPISim() {
    m_index = 0;
  }

  /**
   * Register a callback to be run when this device is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = SPIDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, SPIDataJNI::cancelInitializedCallback);
  }

  /**
   * Check whether this device has been initialized.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return SPIDataJNI.getInitialized(m_index);
  }

  /**
   * Define whether this device has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  public void setInitialized(boolean initialized) {
    SPIDataJNI.setInitialized(m_index, initialized);
  }

  /**
   * Register a callback to be run whenever a `read` operation is executed.
   *
   * @param callback the callback
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerReadCallback(BufferCallback callback) {
    int uid = SPIDataJNI.registerReadCallback(m_index, callback);
    return new CallbackStore(m_index, uid, SPIDataJNI::cancelReadCallback);
  }

  /**
   * Register a callback to be run whenever a `write` operation is executed.
   *
   * @param callback the callback
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerWriteCallback(ConstBufferCallback callback) {
    int uid = SPIDataJNI.registerWriteCallback(m_index, callback);
    return new CallbackStore(m_index, uid, SPIDataJNI::cancelWriteCallback);
  }

  public CallbackStore registerReadAutoReceiveBufferCallback(
      SpiReadAutoReceiveBufferCallback callback) {
    int uid = SPIDataJNI.registerReadAutoReceiveBufferCallback(m_index, callback);
    return new CallbackStore(m_index, uid, SPIDataJNI::cancelReadAutoReceiveBufferCallback);
  }

  /** Reset all simulation data. */
  public void resetData() {
    SPIDataJNI.resetData(m_index);
  }
}
