// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.hal.simulation.PDPDataJNI;
import edu.wpi.first.wpilibj.PowerDistributionPanel;

/** Class to control a simulated Power Distribution Panel (PDP). */
public class PDPSim {
  private final int m_index;

  /** Constructs for the default PDP. */
  public PDPSim() {
    m_index = 0;
  }

  /**
   * Constructs from a PDP module number (CAN ID).
   *
   * @param module module number
   */
  public PDPSim(int module) {
    m_index = module;
  }

  /**
   * Constructs from a PowerDistributionPanel object.
   *
   * @param pdp PowerDistributionPanel to simulate
   */
  public PDPSim(PowerDistributionPanel pdp) {
    m_index = pdp.getModule();
  }

  /**
   * Register a callback to be run when the PDP is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PDPDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PDPDataJNI::cancelInitializedCallback);
  }

  /**
   * Check whether the PDP has been initialized.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return PDPDataJNI.getInitialized(m_index);
  }

  /**
   * Define whether the PDP has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  public void setInitialized(boolean initialized) {
    PDPDataJNI.setInitialized(m_index, initialized);
  }

  /**
   * Register a callback to be run whenever the PDP temperature changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerTemperatureCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PDPDataJNI.registerTemperatureCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PDPDataJNI::cancelTemperatureCallback);
  }

  /**
   * Check the temperature of the PDP.
   *
   * @return the PDP temperature
   */
  public double getTemperature() {
    return PDPDataJNI.getTemperature(m_index);
  }

  /**
   * Define the PDP temperature.
   *
   * @param temperature the new PDP temperature
   */
  public void setTemperature(double temperature) {
    PDPDataJNI.setTemperature(m_index, temperature);
  }

  /**
   * Register a callback to be run whenever the PDP voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerVoltageCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PDPDataJNI.registerVoltageCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PDPDataJNI::cancelVoltageCallback);
  }

  /**
   * Check the PDP voltage.
   *
   * @return the PDP voltage.
   */
  public double getVoltage() {
    return PDPDataJNI.getVoltage(m_index);
  }

  /**
   * Set the PDP voltage.
   *
   * @param voltage the new PDP voltage
   */
  public void setVoltage(double voltage) {
    PDPDataJNI.setVoltage(m_index, voltage);
  }

  /**
   * Register a callback to be run whenever the current of a specific channel changes.
   *
   * @param channel the channel
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerCurrentCallback(
      int channel, NotifyCallback callback, boolean initialNotify) {
    int uid = PDPDataJNI.registerCurrentCallback(m_index, channel, callback, initialNotify);
    return new CallbackStore(m_index, channel, uid, PDPDataJNI::cancelCurrentCallback);
  }

  /**
   * Read the current in one of the PDP channels.
   *
   * @param channel the channel to check
   * @return the current in the given channel
   */
  public double getCurrent(int channel) {
    return PDPDataJNI.getCurrent(m_index, channel);
  }

  /**
   * Change the current in the given channel.
   *
   * @param channel the channel to edit
   * @param current the new current for the channel
   */
  public void setCurrent(int channel, double current) {
    PDPDataJNI.setCurrent(m_index, channel, current);
  }

  /** Reset all PDP simulation data. */
  public void resetData() {
    PDPDataJNI.resetData(m_index);
  }
}
