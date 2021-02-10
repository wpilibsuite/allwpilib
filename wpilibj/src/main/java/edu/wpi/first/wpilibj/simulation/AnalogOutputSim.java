// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.AnalogOutDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.AnalogOutput;

/** Class to control a simulated analog output. */
public class AnalogOutputSim {
  private final int m_index;

  /**
   * Constructs from an AnalogOutput object.
   *
   * @param analogOutput AnalogOutput to simulate
   */
  public AnalogOutputSim(AnalogOutput analogOutput) {
    m_index = analogOutput.getChannel();
  }

  /**
   * Constructs from an analog output channel number.
   *
   * @param channel Channel number
   */
  public AnalogOutputSim(int channel) {
    m_index = channel;
  }

  /**
   * Register a callback to be run whenever the voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerVoltageCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogOutDataJNI.registerVoltageCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogOutDataJNI::cancelVoltageCallback);
  }

  /**
   * Read the analog output voltage.
   *
   * @return the voltage on this analog output
   */
  public double getVoltage() {
    return AnalogOutDataJNI.getVoltage(m_index);
  }

  /**
   * Set the analog output voltage.
   *
   * @param voltage the new voltage on this analog output
   */
  public void setVoltage(double voltage) {
    AnalogOutDataJNI.setVoltage(m_index, voltage);
  }

  /**
   * Register a callback to be run when this analog output is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogOutDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogOutDataJNI::cancelInitializedCallback);
  }

  /**
   * Check whether this analog output has been initialized.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return AnalogOutDataJNI.getInitialized(m_index);
  }

  /**
   * Define whether this analog output has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  public void setInitialized(boolean initialized) {
    AnalogOutDataJNI.setInitialized(m_index, initialized);
  }

  /** Reset all simulation data on this object. */
  public void resetData() {
    AnalogOutDataJNI.resetData(m_index);
  }
}
