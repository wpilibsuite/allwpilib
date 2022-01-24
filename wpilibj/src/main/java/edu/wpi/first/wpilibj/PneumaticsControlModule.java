// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.CTREPCMJNI;
import java.util.HashMap;
import java.util.Map;

/** Module class for controlling a Cross The Road Electronics Pneumatics Control Module. */
public class PneumaticsControlModule implements PneumaticsBase {
  private static class DataStore implements AutoCloseable {
    public final int m_module;
    public final int m_handle;
    private int m_refCount;
    private int m_reservedMask;
    private boolean m_compressorReserved;
    private final Object m_reserveLock = new Object();

    DataStore(int module) {
      m_handle = CTREPCMJNI.initialize(module);
      m_module = module;
      m_handleMap.put(module, this);
    }

    @Override
    public void close() {
      CTREPCMJNI.free(m_handle);
      m_handleMap.remove(m_module);
    }

    public void addRef() {
      m_refCount++;
    }

    public void removeRef() {
      m_refCount--;
      if (m_refCount == 0) {
        this.close();
      }
    }
  }

  private static final Map<Integer, DataStore> m_handleMap = new HashMap<>();
  private static final Object m_handleLock = new Object();

  private static DataStore getForModule(int module) {
    synchronized (m_handleLock) {
      Integer moduleBoxed = module;
      DataStore pcm = m_handleMap.get(moduleBoxed);
      if (pcm == null) {
        pcm = new DataStore(module);
      }
      pcm.addRef();
      return pcm;
    }
  }

  private static void freeModule(DataStore store) {
    synchronized (m_handleLock) {
      store.removeRef();
    }
  }

  private final DataStore m_dataStore;
  private final int m_handle;

  /** Constructs a PneumaticsControlModule with the default id (0). */
  public PneumaticsControlModule() {
    this(SensorUtil.getDefaultCTREPCMModule());
  }

  /**
   * Constructs a PneumaticsControlModule.
   *
   * @param module module number to construct
   */
  public PneumaticsControlModule(int module) {
    m_dataStore = getForModule(module);
    m_handle = m_dataStore.m_handle;
  }

  @Override
  public void close() {
    freeModule(m_dataStore);
  }

  @Override
  public boolean getCompressor() {
    return CTREPCMJNI.getCompressor(m_handle);
  }

  @Override
  public boolean getPressureSwitch() {
    return CTREPCMJNI.getPressureSwitch(m_handle);
  }

  @Override
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
    return m_dataStore.m_module;
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
    synchronized (m_dataStore.m_reserveLock) {
      if ((m_dataStore.m_reservedMask & mask) != 0) {
        return m_dataStore.m_reservedMask & mask;
      }
      m_dataStore.m_reservedMask |= mask;
      return 0;
    }
  }

  @Override
  public void unreserveSolenoids(int mask) {
    synchronized (m_dataStore.m_reserveLock) {
      m_dataStore.m_reservedMask &= ~mask;
    }
  }

  @Override
  public Solenoid makeSolenoid(int channel) {
    return new Solenoid(m_dataStore.m_module, PneumaticsModuleType.CTREPCM, channel);
  }

  @Override
  public DoubleSolenoid makeDoubleSolenoid(int forwardChannel, int reverseChannel) {
    return new DoubleSolenoid(
        m_dataStore.m_module, PneumaticsModuleType.CTREPCM, forwardChannel, reverseChannel);
  }

  @Override
  public Compressor makeCompressor() {
    return new Compressor(m_dataStore.m_module, PneumaticsModuleType.CTREPCM);
  }

  @Override
  public boolean reserveCompressor() {
    synchronized (m_dataStore.m_reserveLock) {
      if (m_dataStore.m_compressorReserved) {
        return false;
      }
      m_dataStore.m_compressorReserved = true;
      return true;
    }
  }

  @Override
  public void unreserveCompressor() {
    synchronized (m_dataStore.m_reserveLock) {
      m_dataStore.m_compressorReserved = false;
    }
  }

  @Override
  public void disableCompressor() {
    CTREPCMJNI.setClosedLoopControl(m_handle, false);
  }

  @Override
  public void enableCompressorDigital() {
    CTREPCMJNI.setClosedLoopControl(m_handle, true);
  }

  @Override
  public void enableCompressorAnalog(double minPressure, double maxPressure) {
    CTREPCMJNI.setClosedLoopControl(m_handle, false);
  }

  @Override
  public void enableCompressorHybrid(double minPressure, double maxPressure) {
    CTREPCMJNI.setClosedLoopControl(m_handle, false);
  }

  @Override
  public CompressorConfigType getCompressorConfigType() {
    return CTREPCMJNI.getClosedLoopControl(m_handle)
        ? CompressorConfigType.Digital
        : CompressorConfigType.Disabled;
  }

  @Override
  public double getAnalogVoltage(int channel) {
    return 0;
  }

  @Override
  public double getPressure(int channel) {
    return 0;
  }
}
