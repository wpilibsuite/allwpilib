/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

import edu.wpi.first.hal.sim.mockdata.PCMDataJNI;

public class PCMSim {
  private int m_index;

  public PCMSim(int index) {
    m_index = index;
  }

  public CallbackStore registerSolenoidInitializedCallback(int channel, NotifyCallback callback, boolean initialNotify) {
    int uid = PCMDataJNI.registerSolenoidInitializedCallback(m_index, channel, callback, initialNotify);
    return new CallbackStore(m_index, channel, uid, PCMDataJNI::cancelSolenoidInitializedCallback);
  }
  public boolean getSolenoidInitialized(int channel) {
    return PCMDataJNI.getSolenoidInitialized(m_index, channel);
  }
  public void setSolenoidInitialized(int channel, boolean solenoidInitialized) {
    PCMDataJNI.setSolenoidInitialized(m_index, channel, solenoidInitialized);
  }

  public CallbackStore registerSolenoidOutputCallback(int channel, NotifyCallback callback, boolean initialNotify) {
    int uid = PCMDataJNI.registerSolenoidOutputCallback(m_index, channel, callback, initialNotify);
    return new CallbackStore(m_index, channel, uid, PCMDataJNI::cancelSolenoidOutputCallback);
  }
  public boolean getSolenoidOutput(int channel) {
    return PCMDataJNI.getSolenoidOutput(m_index, channel);
  }
  public void setSolenoidOutput(int channel, boolean solenoidOutput) {
    PCMDataJNI.setSolenoidOutput(m_index, channel, solenoidOutput);
  }

  public CallbackStore registerCompressorInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PCMDataJNI.registerCompressorInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PCMDataJNI::cancelCompressorInitializedCallback);
  }
  public boolean getCompressorInitialized() {
    return PCMDataJNI.getCompressorInitialized(m_index);
  }
  public void setCompressorInitialized(boolean compressorInitialized) {
    PCMDataJNI.setCompressorInitialized(m_index, compressorInitialized);
  }

  public CallbackStore registerCompressorOnCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PCMDataJNI.registerCompressorOnCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PCMDataJNI::cancelCompressorOnCallback);
  }
  public boolean getCompressorOn() {
    return PCMDataJNI.getCompressorOn(m_index);
  }
  public void setCompressorOn(boolean compressorOn) {
    PCMDataJNI.setCompressorOn(m_index, compressorOn);
  }

  public CallbackStore registerClosedLoopEnabledCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PCMDataJNI.registerClosedLoopEnabledCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PCMDataJNI::cancelClosedLoopEnabledCallback);
  }
  public boolean getClosedLoopEnabled() {
    return PCMDataJNI.getClosedLoopEnabled(m_index);
  }
  public void setClosedLoopEnabled(boolean closedLoopEnabled) {
    PCMDataJNI.setClosedLoopEnabled(m_index, closedLoopEnabled);
  }

  public CallbackStore registerPressureSwitchCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PCMDataJNI.registerPressureSwitchCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PCMDataJNI::cancelPressureSwitchCallback);
  }
  public boolean getPressureSwitch() {
    return PCMDataJNI.getPressureSwitch(m_index);
  }
  public void setPressureSwitch(boolean pressureSwitch) {
    PCMDataJNI.setPressureSwitch(m_index, pressureSwitch);
  }

  public CallbackStore registerCompressorCurrentCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = PCMDataJNI.registerCompressorCurrentCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, PCMDataJNI::cancelCompressorCurrentCallback);
  }
  public double getCompressorCurrent() {
    return PCMDataJNI.getCompressorCurrent(m_index);
  }
  public void setCompressorCurrent(double compressorCurrent) {
    PCMDataJNI.setCompressorCurrent(m_index, compressorCurrent);
  }

  public void resetData() {
    PCMDataJNI.resetData(m_index);
  }
}
