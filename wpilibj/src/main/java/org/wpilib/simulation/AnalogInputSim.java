// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import edu.wpi.first.hal.simulation.AnalogInDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import org.wpilib.AnalogInput;

/** Class to control a simulated analog input. */
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

  /**
   * Register a callback on whether the analog input is initialized.
   *
   * @param callback the callback that will be called whenever the analog input is initialized
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelInitializedCallback);
  }

  /**
   * Check if this analog input has been initialized.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return AnalogInDataJNI.getInitialized(m_index);
  }

  /**
   * Change whether this analog input has been initialized.
   *
   * @param initialized the new value
   */
  public void setInitialized(boolean initialized) {
    AnalogInDataJNI.setInitialized(m_index, initialized);
  }

  /**
   * Register a callback on the number of average bits.
   *
   * @param callback the callback that will be called whenever the number of average bits is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerAverageBitsCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerAverageBitsCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelAverageBitsCallback);
  }

  /**
   * Get the number of average bits.
   *
   * @return the number of average bits
   */
  public int getAverageBits() {
    return AnalogInDataJNI.getAverageBits(m_index);
  }

  /**
   * Change the number of average bits.
   *
   * @param averageBits the new value
   */
  public void setAverageBits(int averageBits) {
    AnalogInDataJNI.setAverageBits(m_index, averageBits);
  }

  /**
   * Register a callback on the amount of oversampling bits.
   *
   * @param callback the callback that will be called whenever the oversampling bits are changed.
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerOversampleBitsCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerOversampleBitsCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelOversampleBitsCallback);
  }

  /**
   * Get the amount of oversampling bits.
   *
   * @return the amount of oversampling bits
   */
  public int getOversampleBits() {
    return AnalogInDataJNI.getOversampleBits(m_index);
  }

  /**
   * Change the amount of oversampling bits.
   *
   * @param oversampleBits the new value
   */
  public void setOversampleBits(int oversampleBits) {
    AnalogInDataJNI.setOversampleBits(m_index, oversampleBits);
  }

  /**
   * Register a callback on the voltage.
   *
   * @param callback the callback that will be called whenever the voltage is changed.
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerVoltageCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerVoltageCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelVoltageCallback);
  }

  /**
   * Get the voltage.
   *
   * @return the voltage
   */
  public double getVoltage() {
    return AnalogInDataJNI.getVoltage(m_index);
  }

  /**
   * Change the voltage.
   *
   * @param voltage the new value
   */
  public void setVoltage(double voltage) {
    AnalogInDataJNI.setVoltage(m_index, voltage);
  }

  /** Reset all simulation data for this object. */
  public void resetData() {
    AnalogInDataJNI.resetData(m_index);
  }
}
