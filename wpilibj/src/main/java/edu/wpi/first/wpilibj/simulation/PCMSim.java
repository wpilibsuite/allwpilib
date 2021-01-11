// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.hal.simulation.PCMDataJNI;
import edu.wpi.first.wpilibj.Compressor;
import edu.wpi.first.wpilibj.SensorUtil;

/** Class to control a simulated Pneumatic Control Module (PCM). */
public class PCMSim {
  private final int m_index;

  /** Constructs for the default PCM. */
  public PCMSim() {
    m_index = SensorUtil.getDefaultSolenoidModule();
  }

  /**
   * Constructs from a PCM module number (CAN ID).
   *
   * @param module module number
   */
  public PCMSim(int module) {
    m_index = module;
  }

  /**
   * Constructs from a Compressor object.
   *
   * @param compressor Compressor connected to PCM to simulate
   */
  public PCMSim(Compressor compressor) {
    m_index = compressor.getModule();
  }

  /**
   * Register a callback to be run when a solenoid is initialized on a channel.
   *
   * @param channel the channel to monitor
   * @param callback the callback
   * @param initialNotify should the callback be run with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerSolenoidInitializedCallback(
      int channel, NotifyCallback callback, boolean initialNotify) {
    int uid =
        PCMDataJNI.registerSolenoidInitializedCallback(m_index, channel, callback, initialNotify);
    return new CallbackStore(m_index, channel, uid, PCMDataJNI::cancelSolenoidInitializedCallback);
  }

  /**
   * Check if a solenoid has been initialized on a specific channel.
   *
   * @param channel the channel to check
   * @return true if initialized
   */
  public boolean getSolenoidInitialized(int channel) {
    return PCMDataJNI.getSolenoidInitialized(m_index, channel);
  }

  /**
   * Define whether a solenoid has been initialized on a specific channel.
   *
   * @param channel the channel
   * @param solenoidInitialized is there a solenoid initialized on that channel
   */
  public void setSolenoidInitialized(int channel, boolean solenoidInitialized) {
    PCMDataJNI.setSolenoidInitialized(m_index, channel, solenoidInitialized);
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
    int uid = PCMDataJNI.registerSolenoidOutputCallback(m_index, channel, callback, initialNotify);
    return new CallbackStore(m_index, channel, uid, PCMDataJNI::cancelSolenoidOutputCallback);
  }

  /**
   * Check the solenoid output on a specific channel.
   *
   * @param channel the channel to check
   * @return the solenoid output
   */
  public boolean getSolenoidOutput(int channel) {
    return PCMDataJNI.getSolenoidOutput(m_index, channel);
  }

  /**
   * Change the solenoid output on a specific channel.
   *
   * @param channel the channel to check
   * @param solenoidOutput the new solenoid output
   */
  public void setSolenoidOutput(int channel, boolean solenoidOutput) {
    PCMDataJNI.setSolenoidOutput(m_index, channel, solenoidOutput);
  }

  /**
   * Register a callback to be run when the compressor is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerCompressorInitializedCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = PCMDataJNI.registerCompressorInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PCMDataJNI::cancelCompressorInitializedCallback);
  }

  /**
   * Check whether the compressor has been initialized.
   *
   * @return true if initialized
   */
  public boolean getCompressorInitialized() {
    return PCMDataJNI.getCompressorInitialized(m_index);
  }

  /**
   * Define whether the compressor has been initialized.
   *
   * @param compressorInitialized whether the compressor is initialized
   */
  public void setCompressorInitialized(boolean compressorInitialized) {
    PCMDataJNI.setCompressorInitialized(m_index, compressorInitialized);
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
    int uid = PCMDataJNI.registerCompressorOnCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PCMDataJNI::cancelCompressorOnCallback);
  }

  /**
   * Check if the compressor is on.
   *
   * @return true if the compressor is active
   */
  public boolean getCompressorOn() {
    return PCMDataJNI.getCompressorOn(m_index);
  }

  /**
   * Set whether the compressor is active.
   *
   * @param compressorOn the new value
   */
  public void setCompressorOn(boolean compressorOn) {
    PCMDataJNI.setCompressorOn(m_index, compressorOn);
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
    int uid = PCMDataJNI.registerClosedLoopEnabledCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PCMDataJNI::cancelClosedLoopEnabledCallback);
  }

  /**
   * Check whether the closed loop compressor control is active.
   *
   * @return true if active
   */
  public boolean getClosedLoopEnabled() {
    return PCMDataJNI.getClosedLoopEnabled(m_index);
  }

  /**
   * Turn on/off the closed loop control of the compressor.
   *
   * @param closedLoopEnabled whether the control loop is active
   */
  public void setClosedLoopEnabled(boolean closedLoopEnabled) {
    PCMDataJNI.setClosedLoopEnabled(m_index, closedLoopEnabled);
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
    int uid = PCMDataJNI.registerPressureSwitchCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PCMDataJNI::cancelPressureSwitchCallback);
  }

  /**
   * Check the value of the pressure switch.
   *
   * @return the pressure switch value
   */
  public boolean getPressureSwitch() {
    return PCMDataJNI.getPressureSwitch(m_index);
  }

  /**
   * Set the value of the pressure switch.
   *
   * @param pressureSwitch the new value
   */
  public void setPressureSwitch(boolean pressureSwitch) {
    PCMDataJNI.setPressureSwitch(m_index, pressureSwitch);
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
    int uid = PCMDataJNI.registerCompressorCurrentCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PCMDataJNI::cancelCompressorCurrentCallback);
  }

  /**
   * Read the compressor current.
   *
   * @return the current of the compressor connected to this module
   */
  public double getCompressorCurrent() {
    return PCMDataJNI.getCompressorCurrent(m_index);
  }

  /**
   * Set the compressor current.
   *
   * @param compressorCurrent the new compressor current
   */
  public void setCompressorCurrent(double compressorCurrent) {
    PCMDataJNI.setCompressorCurrent(m_index, compressorCurrent);
  }

  /** Reset all simulation data for this object. */
  public void resetData() {
    PCMDataJNI.resetData(m_index);
  }
}
