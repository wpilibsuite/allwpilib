// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.hal.simulation.PDPDataJNI;
import edu.wpi.first.wpilibj.PowerDistributionPanel;

/**
 * Class to control a simulated Power Distribution Panel (PDP).
 */
public class PDPSim {
  private final int m_index;

  /**
   * Constructs for the default PDP.
   */
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

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PDPDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PDPDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return PDPDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    PDPDataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerTemperatureCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PDPDataJNI.registerTemperatureCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PDPDataJNI::cancelTemperatureCallback);
  }
  public double getTemperature() {
    return PDPDataJNI.getTemperature(m_index);
  }
  public void setTemperature(double temperature) {
    PDPDataJNI.setTemperature(m_index, temperature);
  }

  public CallbackStore registerVoltageCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PDPDataJNI.registerVoltageCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PDPDataJNI::cancelVoltageCallback);
  }
  public double getVoltage() {
    return PDPDataJNI.getVoltage(m_index);
  }
  public void setVoltage(double voltage) {
    PDPDataJNI.setVoltage(m_index, voltage);
  }

  public CallbackStore registerCurrentCallback(int channel, NotifyCallback callback, boolean initialNotify) {
    int uid = PDPDataJNI.registerCurrentCallback(m_index, channel, callback, initialNotify);
    return new CallbackStore(m_index, channel, uid, PDPDataJNI::cancelCurrentCallback);
  }
  public double getCurrent(int channel) {
    return PDPDataJNI.getCurrent(m_index, channel);
  }
  public void setCurrent(int channel, double current) {
    PDPDataJNI.setCurrent(m_index, channel, current);
  }

  public void resetData() {
    PDPDataJNI.resetData(m_index);
  }
}
