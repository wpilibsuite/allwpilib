// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib;

import org.wpilib.hardware.hal.HAL;
import org.wpilib.networktables.BooleanSubscriber;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.system.SystemServer;

/** This class controls a REV ExpansionHub plugged in over USB to Systemcore. */
public class ExpansionHub implements AutoCloseable {
  private static class DataStore implements AutoCloseable {
    public final int m_usbId;
    private int m_refCount;
    private int m_reservedMotorMask;
    private int m_reservedServoMask;
    private final Object m_reserveLock = new Object();

    private final BooleanSubscriber m_hubConnectedSubscriber;

    DataStore(int usbId) {
      m_usbId = usbId;
      m_storeMap[usbId] = this;

      NetworkTableInstance systemServer = SystemServer.getSystemServer();

      m_hubConnectedSubscriber =
          systemServer.getBooleanTopic("/rhsp/" + usbId + "/connected").subscribe(false);
    }

    @Override
    public void close() {
      m_storeMap[m_usbId] = null;
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

  private static final DataStore[] m_storeMap = new DataStore[4];

  private static void checkUsbId(int usbId) {
    if (usbId < 0 || usbId > 3) {
      throw new IllegalArgumentException("USB Port " + usbId + " out of range");
    }
  }

  private static DataStore getForUsbId(int usbId) {
    checkUsbId(usbId);
    synchronized (m_storeMap) {
      DataStore store = m_storeMap[usbId];
      if (store == null) {
        store = new DataStore(usbId);
      }
      store.addRef();
      return store;
    }
  }

  private static void freeHub(DataStore store) {
    synchronized (m_storeMap) {
      store.removeRef();
    }
  }

  private final DataStore m_dataStore;

  /**
   * Constructs a new ExpansionHub for a given USB ID
   *
   * <p>Multiple instances can be constructed, but will point to the same backing object with a ref
   * count.
   *
   * @param usbId The USB Port ID the hub is plugged into.
   */
  public ExpansionHub(int usbId) {
    m_dataStore = getForUsbId(usbId);
  }

  /**
   * Closes an ExpansionHub object. Will not close any other instances until the last instance is
   * closed.
   */
  @Override
  public void close() {
    freeHub(m_dataStore);
  }

  boolean checkServoChannel(int channel) {
    return channel >= 0 && channel < 6;
  }

  boolean checkAndReserveServo(int channel) {
    int mask = 1 << channel;
    synchronized (m_dataStore.m_reserveLock) {
      if ((m_dataStore.m_reservedServoMask & mask) != 0) {
        return false;
      }
      m_dataStore.m_reservedServoMask |= mask;
      return true;
    }
  }

  void unreserveServo(int channel) {
    int mask = 1 << channel;
    synchronized (m_dataStore.m_reserveLock) {
      m_dataStore.m_reservedServoMask &= ~mask;
    }
  }

  boolean checkMotorChannel(int channel) {
    return channel >= 0 && channel < 4;
  }

  boolean checkAndReserveMotor(int channel) {
    int mask = 1 << channel;
    synchronized (m_dataStore.m_reserveLock) {
      if ((m_dataStore.m_reservedMotorMask & mask) != 0) {
        return false;
      }
      m_dataStore.m_reservedMotorMask |= mask;
      return true;
    }
  }

  void unreserveMotor(int channel) {
    int mask = 1 << channel;
    synchronized (m_dataStore.m_reserveLock) {
      m_dataStore.m_reservedMotorMask &= ~mask;
    }
  }

  void reportUsage(String device, String data) {
    HAL.reportUsage("ExpansionHub[" + m_dataStore.m_usbId + "]/" + device, data);
  }

  /**
   * Constructs a servo at the requested channel on this hub.
   *
   * <p>Only a single instance of each servo per hub can be constructed at a time.
   *
   * @param channel The servo channel
   * @return Servo object
   */
  public ExpansionHubServo makeServo(int channel) {
    return new ExpansionHubServo(m_dataStore.m_usbId, channel);
  }

  /**
   * Constructs a motor at the requested channel on this hub.
   *
   * <p>Only a single instance of each motor per hub can be constructed at a time.
   *
   * @param channel The motor channel
   * @return Motor object
   */
  public ExpansionHubMotor makeMotor(int channel) {
    return new ExpansionHubMotor(m_dataStore.m_usbId, channel);
  }

  /**
   * Gets if the hub is currently connected over USB.
   *
   * @return True if hub connection, otherwise false
   */
  public boolean isHubConnected() {
    return m_dataStore.m_hubConnectedSubscriber.get(false);
  }

  /**
   * Gets the USB ID of this hub.
   *
   * @return The USB ID
   */
  public int getUsbId() {
    return m_dataStore.m_usbId;
  }
}
