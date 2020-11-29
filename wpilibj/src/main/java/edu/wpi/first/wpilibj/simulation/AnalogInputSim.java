/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.AnalogInDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.AnalogInput;

/**
 * Class to control a simulated analog input.
 */
public class AnalogInputSim {
  private final int m_index;

  /**
   * Constructs from an AnalogInput object.
   *
   * @param analogInput AnalogInput to simulate
   */
  public AnalogInputSim(AnalogInput analogInput) {
    m_index = analogInput.getChannel();
  }

  /**
   * Constructs from an analog input channel number.
   *
   * @param channel Channel number
   */
  public AnalogInputSim(int channel) {
    m_index = channel;
  }

  public String getDisplayName() {
    return AnalogInDataJNI.getDisplayName(m_index);
  }

  public void setDisplayName(String displayName) {
    AnalogInDataJNI.setDisplayName(m_index, displayName);
  }


  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return AnalogInDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    AnalogInDataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerAverageBitsCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerAverageBitsCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelAverageBitsCallback);
  }
  public int getAverageBits() {
    return AnalogInDataJNI.getAverageBits(m_index);
  }
  public void setAverageBits(int averageBits) {
    AnalogInDataJNI.setAverageBits(m_index, averageBits);
  }

  public CallbackStore registerOversampleBitsCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerOversampleBitsCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelOversampleBitsCallback);
  }
  public int getOversampleBits() {
    return AnalogInDataJNI.getOversampleBits(m_index);
  }
  public void setOversampleBits(int oversampleBits) {
    AnalogInDataJNI.setOversampleBits(m_index, oversampleBits);
  }

  public CallbackStore registerVoltageCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerVoltageCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelVoltageCallback);
  }
  public double getVoltage() {
    return AnalogInDataJNI.getVoltage(m_index);
  }
  public void setVoltage(double voltage) {
    AnalogInDataJNI.setVoltage(m_index, voltage);
  }

  public CallbackStore registerAccumulatorInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerAccumulatorInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelAccumulatorInitializedCallback);
  }
  public boolean getAccumulatorInitialized() {
    return AnalogInDataJNI.getAccumulatorInitialized(m_index);
  }
  public void setAccumulatorInitialized(boolean accumulatorInitialized) {
    AnalogInDataJNI.setAccumulatorInitialized(m_index, accumulatorInitialized);
  }

  public CallbackStore registerAccumulatorValueCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerAccumulatorValueCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelAccumulatorValueCallback);
  }
  public long getAccumulatorValue() {
    return AnalogInDataJNI.getAccumulatorValue(m_index);
  }
  public void setAccumulatorValue(long accumulatorValue) {
    AnalogInDataJNI.setAccumulatorValue(m_index, accumulatorValue);
  }

  public CallbackStore registerAccumulatorCountCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerAccumulatorCountCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelAccumulatorCountCallback);
  }
  public long getAccumulatorCount() {
    return AnalogInDataJNI.getAccumulatorCount(m_index);
  }
  public void setAccumulatorCount(long accumulatorCount) {
    AnalogInDataJNI.setAccumulatorCount(m_index, accumulatorCount);
  }

  public CallbackStore registerAccumulatorCenterCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerAccumulatorCenterCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelAccumulatorCenterCallback);
  }
  public int getAccumulatorCenter() {
    return AnalogInDataJNI.getAccumulatorCenter(m_index);
  }
  public void setAccumulatorCenter(int accumulatorCenter) {
    AnalogInDataJNI.setAccumulatorCenter(m_index, accumulatorCenter);
  }

  public CallbackStore registerAccumulatorDeadbandCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerAccumulatorDeadbandCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelAccumulatorDeadbandCallback);
  }
  public int getAccumulatorDeadband() {
    return AnalogInDataJNI.getAccumulatorDeadband(m_index);
  }
  public void setAccumulatorDeadband(int accumulatorDeadband) {
    AnalogInDataJNI.setAccumulatorDeadband(m_index, accumulatorDeadband);
  }

  public void resetData() {
    AnalogInDataJNI.resetData(m_index);
  }
}
