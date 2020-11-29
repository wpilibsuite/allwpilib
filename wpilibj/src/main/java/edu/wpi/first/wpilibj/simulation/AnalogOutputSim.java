/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.AnalogOutDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.AnalogOutput;

/**
 * Class to control a simulated analog output.
 */
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

  public String getDisplayName() {
    return AnalogOutDataJNI.getDisplayName(m_index);
  }

  public void setDisplayName(String displayName) {
    AnalogOutDataJNI.setDisplayName(m_index, displayName);
  }

  public CallbackStore registerVoltageCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogOutDataJNI.registerVoltageCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogOutDataJNI::cancelVoltageCallback);
  }
  public double getVoltage() {
    return AnalogOutDataJNI.getVoltage(m_index);
  }
  public void setVoltage(double voltage) {
    AnalogOutDataJNI.setVoltage(m_index, voltage);
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogOutDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogOutDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return AnalogOutDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    AnalogOutDataJNI.setInitialized(m_index, initialized);
  }

  public void resetData() {
    AnalogOutDataJNI.resetData(m_index);
  }
}
