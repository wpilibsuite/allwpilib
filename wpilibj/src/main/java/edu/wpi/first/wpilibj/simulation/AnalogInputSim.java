// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.AnalogInDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.AnalogInput;

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
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
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
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
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
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
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
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
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

  /**
   * Register a callback on whether the accumulator is initialized.
   *
   * @param callback the callback that will be called whenever the accumulator is initialized
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerAccumulatorInitializedCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid =
        AnalogInDataJNI.registerAccumulatorInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelAccumulatorInitializedCallback);
  }

  /**
   * Check if the accumulator has been initialized.
   *
   * @return true if initialized
   */
  public boolean getAccumulatorInitialized() {
    return AnalogInDataJNI.getAccumulatorInitialized(m_index);
  }

  /**
   * Change whether the accumulator has been initialized.
   *
   * @param accumulatorInitialized the new value
   */
  public void setAccumulatorInitialized(boolean accumulatorInitialized) {
    AnalogInDataJNI.setAccumulatorInitialized(m_index, accumulatorInitialized);
  }

  /**
   * Register a callback on the accumulator value.
   *
   * @param callback the callback that will be called whenever the accumulator value is changed.
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerAccumulatorValueCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerAccumulatorValueCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelAccumulatorValueCallback);
  }

  /**
   * Get the accumulator value.
   *
   * @return the accumulator value
   */
  public long getAccumulatorValue() {
    return AnalogInDataJNI.getAccumulatorValue(m_index);
  }

  /**
   * Change the accumulator value.
   *
   * @param accumulatorValue the new value
   */
  public void setAccumulatorValue(long accumulatorValue) {
    AnalogInDataJNI.setAccumulatorValue(m_index, accumulatorValue);
  }

  /**
   * Register a callback on the accumulator count.
   *
   * @param callback the callback that will be called whenever the accumulator count is changed.
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerAccumulatorCountCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerAccumulatorCountCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelAccumulatorCountCallback);
  }

  /**
   * Get the accumulator count.
   *
   * @return the accumulator count.
   */
  public long getAccumulatorCount() {
    return AnalogInDataJNI.getAccumulatorCount(m_index);
  }

  /**
   * Change the accumulator count.
   *
   * @param accumulatorCount the new count.
   */
  public void setAccumulatorCount(long accumulatorCount) {
    AnalogInDataJNI.setAccumulatorCount(m_index, accumulatorCount);
  }

  /**
   * Register a callback on the accumulator center.
   *
   * @param callback the callback that will be called whenever the accumulator center is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerAccumulatorCenterCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerAccumulatorCenterCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelAccumulatorCenterCallback);
  }

  /**
   * Get the accumulator center.
   *
   * @return the accumulator center
   */
  public int getAccumulatorCenter() {
    return AnalogInDataJNI.getAccumulatorCenter(m_index);
  }

  /**
   * Change the accumulator center.
   *
   * @param accumulatorCenter the new center
   */
  public void setAccumulatorCenter(int accumulatorCenter) {
    AnalogInDataJNI.setAccumulatorCenter(m_index, accumulatorCenter);
  }

  /**
   * Register a callback on the accumulator deadband.
   *
   * @param callback the callback that will be called whenever the accumulator deadband is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerAccumulatorDeadbandCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogInDataJNI.registerAccumulatorDeadbandCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogInDataJNI::cancelAccumulatorDeadbandCallback);
  }

  /**
   * Get the accumulator deadband.
   *
   * @return the accumulator deadband
   */
  public int getAccumulatorDeadband() {
    return AnalogInDataJNI.getAccumulatorDeadband(m_index);
  }

  /**
   * Change the accumulator deadband.
   *
   * @param accumulatorDeadband the new deadband
   */
  public void setAccumulatorDeadband(int accumulatorDeadband) {
    AnalogInDataJNI.setAccumulatorDeadband(m_index, accumulatorDeadband);
  }

  /** Reset all simulation data for this object. */
  public void resetData() {
    AnalogInDataJNI.resetData(m_index);
  }
}
