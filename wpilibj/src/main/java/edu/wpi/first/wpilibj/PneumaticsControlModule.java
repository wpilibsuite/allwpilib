// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.CTREPCMJNI;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

public class PneumaticsControlModule implements PneumaticsBase, Sendable {
  private final int m_handle;
  private final int m_module;
  private int m_reservedMask;
  private final Object m_reserveLock = new Object();

  public PneumaticsControlModule() {
    this(SensorUtil.getDefaultCTREPCMModule());
  }

  /**
   * Constructs a PneumaticsControlModule.
   *
   * @param module module number to construct
   */
  public PneumaticsControlModule(int module) {
    m_handle = CTREPCMJNI.initialize(module);
    m_module = module;

    SendableRegistry.addLW(this, "Compressor", module);
  }

  @Override
  public void close() {
    CTREPCMJNI.free(m_handle);
    SendableRegistry.remove(this);
  }

  public boolean getCompressor() {
    return CTREPCMJNI.getCompressor(m_handle);
  }

  public void setClosedLoopControl(boolean enabled) {
    CTREPCMJNI.setClosedLoopControl(m_handle, enabled);
  }

  public boolean getClosedLoopControl() {
    return CTREPCMJNI.getClosedLoopControl(m_handle);
  }

  public boolean getPressureSwitch() {
    return CTREPCMJNI.getPressureSwitch(m_handle);
  }

  public double getCompressorCurrent() {
    return CTREPCMJNI.getCompressorCurrent(m_handle);
  }

  public boolean getCompressorCurrentTooHighFault() {
    return CTREPCMJNI.getCompressorCurrentTooHighFault(m_handle);
  }

  public boolean getCompressorCurrentTooHighStickyFault() {
    return CTREPCMJNI.getCompressorCurrentTooHighStickyFault(m_handle);
  }

  public boolean getCompressorShortedFault() {
    return CTREPCMJNI.getCompressorShortedFault(m_handle);
  }

  public boolean getCompressorShortedStickyFault() {
    return CTREPCMJNI.getCompressorShortedStickyFault(m_handle);
  }

  public boolean getCompressorNotConnectedFault() {
    return CTREPCMJNI.getCompressorNotConnectedFault(m_handle);
  }

  public boolean getCompressorNotConnectedStickyFault() {
    return CTREPCMJNI.getCompressorNotConnectedStickyFault(m_handle);
  }

  @Override
  public void setSolenoids(int mask, int values) {
    CTREPCMJNI.setSolenoids(m_handle, mask, values);
  }

  @Override
  public int getSolenoids() {
    return CTREPCMJNI.getSolenoids(m_handle);
  }

  @Override
  public int getModuleNumber() {
    return m_module;
  }

  @Override
  public int getSolenoidDisabledList() {
    return CTREPCMJNI.getSolenoidDisabledList(m_handle);
  }

  public boolean getSolenoidVoltageFault() {
    return CTREPCMJNI.getSolenoidVoltageFault(m_handle);
  }

  public boolean getSolenoidVoltageStickyFault() {
    return CTREPCMJNI.getSolenoidVoltageStickyFault(m_handle);
  }

  public void clearAllStickyFaults() {
    CTREPCMJNI.clearAllStickyFaults(m_handle);
  }

  @Override
  public void fireOneShot(int index) {
    CTREPCMJNI.fireOneShot(m_handle, index);
  }

  @Override
  public void setOneShotDuration(int index, int durMs) {
    CTREPCMJNI.setOneShotDuration(m_handle, index, durMs);
  }

  @Override
  public boolean checkSolenoidChannel(int channel) {
    return CTREPCMJNI.checkSolenoidChannel(channel);
  }

  @Override
  public int checkAndReserveSolenoids(int mask) {
    synchronized (m_reserveLock) {
      if ((m_reservedMask & mask) != 0) {
        return m_reservedMask & mask;
      }
      m_reservedMask |= mask;
      return 0;
    }
  }

  @Override
  public void unreserveSolenoids(int mask) {
    synchronized (m_reserveLock) {
      m_reservedMask &= ~mask;
    }
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Compressor");
    builder.addBooleanProperty(
        "Closed Loop Control", this::getClosedLoopControl, this::setClosedLoopControl);
    builder.addBooleanProperty("Enabled", this::getCompressor, null);
    builder.addBooleanProperty("Pressure switch", this::getPressureSwitch, null);
  }
}
