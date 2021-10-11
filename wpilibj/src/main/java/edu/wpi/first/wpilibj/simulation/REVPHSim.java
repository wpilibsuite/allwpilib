// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.hal.simulation.REVPHDataJNI;
import edu.wpi.first.wpilibj.PneumaticsBase;
import edu.wpi.first.wpilibj.SensorUtil;

/** Class to control a simulated PneumaticsHub (PH). */
@SuppressWarnings("AbbreviationAsWordInName")
public class REVPHSim {
  private final int m_index;

  /** Constructs for the default PH. */
  public REVPHSim() {
    m_index = SensorUtil.getDefaultREVPHModule();
  }

  /**
   * Constructs from a PH module number (CAN ID).
   *
   * @param module module number
   */
  public REVPHSim(int module) {
    m_index = module;
  }

  /**
   * Constructs from a Compressor object.
   *
   * @param module PCM module to simulate
   */
  public REVPHSim(PneumaticsBase module) {
    m_index = module.getModuleNumber();
  }

  /**
   * Register a callback to be run when the solenoid output on a channel changes.
   *
   * @param channel the channel to monitor
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerSolenoidOutputCallback(
      int channel, NotifyCallback callback, boolean initialNotify) {
    int uid =
        REVPHDataJNI.registerSolenoidOutputCallback(m_index, channel, callback, initialNotify);
    return new CallbackStore(m_index, channel, uid, REVPHDataJNI::cancelSolenoidOutputCallback);
  }

  /**
   * Check the solenoid output on a specific channel.
   *
   * @param channel the channel to check
   * @return the solenoid output
   */
  public boolean getSolenoidOutput(int channel) {
    return REVPHDataJNI.getSolenoidOutput(m_index, channel);
  }

  /**
   * Change the solenoid output on a specific channel.
   *
   * @param channel the channel to check
   * @param solenoidOutput the new solenoid output
   */
  public void setSolenoidOutput(int channel, boolean solenoidOutput) {
    REVPHDataJNI.setSolenoidOutput(m_index, channel, solenoidOutput);
  }

  /**
   * Register a callback to be run when the compressor is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = REVPHDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, REVPHDataJNI::cancelInitializedCallback);
  }

  /**
   * Check whether the compressor has been initialized.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return REVPHDataJNI.getInitialized(m_index);
  }

  /**
   * Define whether the compressor has been initialized.
   *
   * @param initialized whether the compressor is initialized
   */
  public void setInitialized(boolean initialized) {
    REVPHDataJNI.setInitialized(m_index, initialized);
  }

  /**
   * Register a callback to be run when the compressor activates.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerCompressorOnCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = REVPHDataJNI.registerCompressorOnCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, REVPHDataJNI::cancelCompressorOnCallback);
  }

  /**
   * Check if the compressor is on.
   *
   * @return true if the compressor is active
   */
  public boolean getCompressorOn() {
    return REVPHDataJNI.getCompressorOn(m_index);
  }

  /**
   * Set whether the compressor is active.
   *
   * @param compressorOn the new value
   */
  public void setCompressorOn(boolean compressorOn) {
    REVPHDataJNI.setCompressorOn(m_index, compressorOn);
  }

  /**
   * Register a callback to be run whenever the closed loop state changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerClosedLoopEnabledCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = REVPHDataJNI.registerClosedLoopEnabledCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, REVPHDataJNI::cancelClosedLoopEnabledCallback);
  }

  /**
   * Check whether the closed loop compressor control is active.
   *
   * @return true if active
   */
  public boolean getClosedLoopEnabled() {
    return REVPHDataJNI.getClosedLoopEnabled(m_index);
  }

  /**
   * Turn on/off the closed loop control of the compressor.
   *
   * @param closedLoopEnabled whether the control loop is active
   */
  public void setClosedLoopEnabled(boolean closedLoopEnabled) {
    REVPHDataJNI.setClosedLoopEnabled(m_index, closedLoopEnabled);
  }

  /**
   * Register a callback to be run whenever the pressure switch value changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerPressureSwitchCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = REVPHDataJNI.registerPressureSwitchCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, REVPHDataJNI::cancelPressureSwitchCallback);
  }

  /**
   * Check the value of the pressure switch.
   *
   * @return the pressure switch value
   */
  public boolean getPressureSwitch() {
    return REVPHDataJNI.getPressureSwitch(m_index);
  }

  /**
   * Set the value of the pressure switch.
   *
   * @param pressureSwitch the new value
   */
  public void setPressureSwitch(boolean pressureSwitch) {
    REVPHDataJNI.setPressureSwitch(m_index, pressureSwitch);
  }

  /**
   * Register a callback to be run whenever the compressor current changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerCompressorCurrentCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = REVPHDataJNI.registerCompressorCurrentCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, REVPHDataJNI::cancelCompressorCurrentCallback);
  }

  /**
   * Read the compressor current.
   *
   * @return the current of the compressor connected to this module
   */
  public double getCompressorCurrent() {
    return REVPHDataJNI.getCompressorCurrent(m_index);
  }

  /**
   * Set the compressor current.
   *
   * @param compressorCurrent the new compressor current
   */
  public void setCompressorCurrent(double compressorCurrent) {
    REVPHDataJNI.setCompressorCurrent(m_index, compressorCurrent);
  }

  /** Reset all simulation data for this object. */
  public void resetData() {
    REVPHDataJNI.resetData(m_index);
  }
}
