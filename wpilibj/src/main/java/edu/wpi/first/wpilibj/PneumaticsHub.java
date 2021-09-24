// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.REVPHJNI;
import java.util.HashMap;
import java.util.Map;

/** Module class for controlling a REV Robotics Pneumatics Hub. */
public class PneumaticsHub implements PneumaticsBase {
  private static class DataStore implements AutoCloseable {
    public final int m_module;
    public final int m_handle;
    private int m_refCount;
    private int m_reservedMask;
    private boolean m_compressorReserved;
    private final Object m_reserveLock = new Object();

    DataStore(int module) {
      m_handle = REVPHJNI.initialize(module);
      m_module = module;
      m_handleMap.put(module, this);
    }

    @Override
    public void close() {
      REVPHJNI.free(m_handle);
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

  /** Constructs a PneumaticsHub with the default id (1). */
  public PneumaticsHub() {
    this(SensorUtil.getDefaultREVPHModule());
  }

  /**
   * Constructs a PneumaticsHub.
   *
   * @param module module number to construct
   */
  public PneumaticsHub(int module) {
    m_dataStore = getForModule(module);
    m_handle = m_dataStore.m_handle;
  }

  @Override
  public void close() {
    freeModule(m_dataStore);
  }

  @Override
  public boolean getCompressor() {
    return REVPHJNI.getCompressor(m_handle);
  }

  @Override
  public void setClosedLoopControl(boolean enabled) {
    REVPHJNI.setClosedLoopControl(m_handle, enabled);
  }

  @Override
  public boolean getClosedLoopControl() {
    return REVPHJNI.getClosedLoopControl(m_handle);
  }

  @Override
  public boolean getPressureSwitch() {
    return REVPHJNI.getPressureSwitch(m_handle);
  }

  @Override
  public double getCompressorCurrent() {
    return REVPHJNI.getCompressorCurrent(m_handle);
  }

  @Override
  public void setSolenoids(int mask, int values) {
    REVPHJNI.setSolenoids(m_handle, mask, values);
  }

  @Override
  public int getSolenoids() {
    return REVPHJNI.getSolenoids(m_handle);
  }

  @Override
  public int getModuleNumber() {
    return m_dataStore.m_module;
  }

  @Override
  public void fireOneShot(int index) {
    // TODO Combine APIs
    //REVPHJNI.fireOneShot(m_handle, index, durMs);
  }

  @Override
  public void setOneShotDuration(int index, int durMs) {
    // TODO Combine APIs
    //REVPHJNI.setOneShotDuration(m_handle, index, durMs);
  }

  @Override
  public boolean checkSolenoidChannel(int channel) {
    return REVPHJNI.checkSolenoidChannel(channel);
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
    return new Solenoid(m_dataStore.m_module, PneumaticsModuleType.REVPH, channel);
  }

  @Override
  public DoubleSolenoid makeDoubleSolenoid(int forwardChannel, int reverseChannel) {
    return new DoubleSolenoid(
        m_dataStore.m_module, PneumaticsModuleType.REVPH, forwardChannel, reverseChannel);
  }

  @Override
  public Compressor makeCompressor() {
    return new Compressor(m_dataStore.m_module, PneumaticsModuleType.REVPH);
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
  public int getSolenoidDisabledList() {
    // TODO Get this working
    return 0;
  }
}
