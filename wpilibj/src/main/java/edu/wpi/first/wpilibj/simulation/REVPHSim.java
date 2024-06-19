// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.hal.simulation.REVPHDataJNI;
import edu.wpi.first.wpilibj.PneumaticHub;
import edu.wpi.first.wpilibj.SensorUtil;

/** Class to control a simulated PneumaticHub (PH). */
public class REVPHSim extends PneumaticsBaseSim {
  /** Constructs for the default PH. */
  public REVPHSim() {
    super(SensorUtil.getDefaultREVPHModule());
  }

  /**
   * Constructs from a PH module number (CAN ID).
   *
   * @param module module number
   */
  public REVPHSim(int module) {
    super(module);
  }

  /**
   * Constructs from a PneumaticHum object.
   *
   * @param module PCM module to simulate
   */
  public REVPHSim(PneumaticHub module) {
    super(module);
  }

  /**
   * Check whether the closed loop compressor control is active.
   *
   * @return config type
   */
  public int getCompressorConfigType() {
    return REVPHDataJNI.getCompressorConfigType(m_index);
  }

  /**
   * Turn on/off the closed loop control of the compressor.
   *
   * @param compressorConfigType compressor config type
   */
  public void setCompressorConfigType(int compressorConfigType) {
    REVPHDataJNI.setCompressorConfigType(m_index, compressorConfigType);
  }

  /**
   * Register a callback to be run whenever the closed loop state changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerCompressorConfigTypeCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = REVPHDataJNI.registerCompressorConfigTypeCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, REVPHDataJNI::cancelCompressorConfigTypeCallback);
  }

  @Override
  public boolean getInitialized() {
    return REVPHDataJNI.getInitialized(m_index);
  }

  @Override
  public void setInitialized(boolean initialized) {
    REVPHDataJNI.setInitialized(m_index, initialized);
  }

  @Override
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = REVPHDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, REVPHDataJNI::cancelInitializedCallback);
  }

  @Override
  public boolean getCompressorOn() {
    return REVPHDataJNI.getCompressorOn(m_index);
  }

  @Override
  public void setCompressorOn(boolean compressorOn) {
    REVPHDataJNI.setCompressorOn(m_index, compressorOn);
  }

  @Override
  public CallbackStore registerCompressorOnCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = REVPHDataJNI.registerCompressorOnCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, REVPHDataJNI::cancelCompressorOnCallback);
  }

  @Override
  public boolean getSolenoidOutput(int channel) {
    return REVPHDataJNI.getSolenoidOutput(m_index, channel);
  }

  @Override
  public void setSolenoidOutput(int channel, boolean solenoidOutput) {
    REVPHDataJNI.setSolenoidOutput(m_index, channel, solenoidOutput);
  }

  @Override
  public CallbackStore registerSolenoidOutputCallback(
      int channel, NotifyCallback callback, boolean initialNotify) {
    int uid =
        REVPHDataJNI.registerSolenoidOutputCallback(m_index, channel, callback, initialNotify);
    return new CallbackStore(m_index, channel, uid, REVPHDataJNI::cancelSolenoidOutputCallback);
  }

  @Override
  public boolean getPressureSwitch() {
    return REVPHDataJNI.getPressureSwitch(m_index);
  }

  @Override
  public void setPressureSwitch(boolean pressureSwitch) {
    REVPHDataJNI.setPressureSwitch(m_index, pressureSwitch);
  }

  @Override
  public CallbackStore registerPressureSwitchCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = REVPHDataJNI.registerPressureSwitchCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, REVPHDataJNI::cancelPressureSwitchCallback);
  }

  @Override
  public double getCompressorCurrent() {
    return REVPHDataJNI.getCompressorCurrent(m_index);
  }

  @Override
  public void setCompressorCurrent(double compressorCurrent) {
    REVPHDataJNI.setCompressorCurrent(m_index, compressorCurrent);
  }

  @Override
  public CallbackStore registerCompressorCurrentCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = REVPHDataJNI.registerCompressorCurrentCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, REVPHDataJNI::cancelCompressorCurrentCallback);
  }

  @Override
  public void resetData() {
    REVPHDataJNI.resetData(m_index);
  }
}
