// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.CTREPCMDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.PneumaticsControlModule;
import edu.wpi.first.wpilibj.SensorUtil;

/** Class to control a simulated Pneumatic Control Module (PCM). */
public class CTREPCMSim extends PneumaticsBaseSim {
  /** Constructs for the default PCM. */
  public CTREPCMSim() {
    super(SensorUtil.getDefaultCTREPCMModule());
  }

  /**
   * Constructs from a PCM module number (CAN ID).
   *
   * @param module module number
   */
  public CTREPCMSim(int module) {
    super(module);
  }

  /**
   * Constructs from a PneumaticsControlModule object.
   *
   * @param module PCM module to simulate
   */
  public CTREPCMSim(PneumaticsControlModule module) {
    super(module);
  }

  /**
   * Check whether the closed loop compressor control is active.
   *
   * @return true if active
   */
  public boolean getClosedLoopEnabled() {
    return CTREPCMDataJNI.getClosedLoopEnabled(m_index);
  }

  /**
   * Turn on/off the closed loop control of the compressor.
   *
   * @param closedLoopEnabled whether the control loop is active
   */
  public void setClosedLoopEnabled(boolean closedLoopEnabled) {
    CTREPCMDataJNI.setClosedLoopEnabled(m_index, closedLoopEnabled);
  }

  /**
   * Register a callback to be run whenever the closed loop state changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public CallbackStore registerClosedLoopEnabledCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = CTREPCMDataJNI.registerClosedLoopEnabledCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, CTREPCMDataJNI::cancelClosedLoopEnabledCallback);
  }

  @Override
  public boolean getInitialized() {
    return CTREPCMDataJNI.getInitialized(m_index);
  }

  @Override
  public void setInitialized(boolean initialized) {
    CTREPCMDataJNI.setInitialized(m_index, initialized);
  }

  @Override
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = CTREPCMDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, CTREPCMDataJNI::cancelInitializedCallback);
  }

  @Override
  public boolean getCompressorOn() {
    return CTREPCMDataJNI.getCompressorOn(m_index);
  }

  @Override
  public void setCompressorOn(boolean compressorOn) {
    CTREPCMDataJNI.setCompressorOn(m_index, compressorOn);
  }

  @Override
  public CallbackStore registerCompressorOnCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = CTREPCMDataJNI.registerCompressorOnCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, CTREPCMDataJNI::cancelCompressorOnCallback);
  }

  @Override
  public boolean getSolenoidOutput(int channel) {
    return CTREPCMDataJNI.getSolenoidOutput(m_index, channel);
  }

  @Override
  public void setSolenoidOutput(int channel, boolean solenoidOutput) {
    CTREPCMDataJNI.setSolenoidOutput(m_index, channel, solenoidOutput);
  }

  @Override
  public CallbackStore registerSolenoidOutputCallback(
      int channel, NotifyCallback callback, boolean initialNotify) {
    int uid =
        CTREPCMDataJNI.registerSolenoidOutputCallback(m_index, channel, callback, initialNotify);
    return new CallbackStore(m_index, channel, uid, CTREPCMDataJNI::cancelSolenoidOutputCallback);
  }

  @Override
  public boolean getPressureSwitch() {
    return CTREPCMDataJNI.getPressureSwitch(m_index);
  }

  @Override
  public void setPressureSwitch(boolean pressureSwitch) {
    CTREPCMDataJNI.setPressureSwitch(m_index, pressureSwitch);
  }

  @Override
  public CallbackStore registerPressureSwitchCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = CTREPCMDataJNI.registerPressureSwitchCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, CTREPCMDataJNI::cancelPressureSwitchCallback);
  }

  @Override
  public double getCompressorCurrent() {
    return CTREPCMDataJNI.getCompressorCurrent(m_index);
  }

  @Override
  public void setCompressorCurrent(double compressorCurrent) {
    CTREPCMDataJNI.setCompressorCurrent(m_index, compressorCurrent);
  }

  @Override
  public CallbackStore registerCompressorCurrentCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = CTREPCMDataJNI.registerCompressorCurrentCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, CTREPCMDataJNI::cancelCompressorCurrentCallback);
  }

  @Override
  public void resetData() {
    CTREPCMDataJNI.resetData(m_index);
  }
}
