/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.DIODataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;

/**
 * Class to control a simulated digital input or output.
 */
public class DIOSim {
  private final int m_index;

  /**
   * Constructs from a DigitalInput object.
   *
   * @param input DigitalInput to simulate
   */
  public DIOSim(DigitalInput input) {
    m_index = input.getChannel();
  }

  /**
   * Constructs from a DigitalOutput object.
   *
   * @param output DigitalOutput to simulate
   */
  public DIOSim(DigitalOutput output) {
    m_index = output.getChannel();
  }

  /**
   * Constructs from an digital I/O channel number.
   *
   * @param channel Channel number
   */
  public DIOSim(int channel) {
    m_index = channel;
  }


  public String getDisplayName() {
    return DIODataJNI.getDisplayName(m_index);
  }

  public void setDisplayName(String displayName) {
    DIODataJNI.setDisplayName(m_index, displayName);
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DIODataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DIODataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return DIODataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    DIODataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerValueCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DIODataJNI.registerValueCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DIODataJNI::cancelValueCallback);
  }
  public boolean getValue() {
    return DIODataJNI.getValue(m_index);
  }
  public void setValue(boolean value) {
    DIODataJNI.setValue(m_index, value);
  }

  public CallbackStore registerPulseLengthCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DIODataJNI.registerPulseLengthCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DIODataJNI::cancelPulseLengthCallback);
  }
  public double getPulseLength() {
    return DIODataJNI.getPulseLength(m_index);
  }
  public void setPulseLength(double pulseLength) {
    DIODataJNI.setPulseLength(m_index, pulseLength);
  }

  public CallbackStore registerIsInputCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DIODataJNI.registerIsInputCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DIODataJNI::cancelIsInputCallback);
  }
  public boolean getIsInput() {
    return DIODataJNI.getIsInput(m_index);
  }
  public void setIsInput(boolean isInput) {
    DIODataJNI.setIsInput(m_index, isInput);
  }

  public CallbackStore registerFilterIndexCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DIODataJNI.registerFilterIndexCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, DIODataJNI::cancelFilterIndexCallback);
  }
  public int getFilterIndex() {
    return DIODataJNI.getFilterIndex(m_index);
  }
  public void setFilterIndex(int filterIndex) {
    DIODataJNI.setFilterIndex(m_index, filterIndex);
  }

  public void resetData() {
    DIODataJNI.resetData(m_index);
  }
}
