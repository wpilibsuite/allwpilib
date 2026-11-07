// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import org.wpilib.datalog.DataLog;
import org.wpilib.driverstation.internal.DriverStationBackend;

/** Provides access to Driver Station functionality. */
public final class DriverStation {
  private DriverStation() {}

  private static final Lock m_dsLock = new ReentrantLock();
  private static final GenericHID[] m_hids = new GenericHID[DriverStationBackend.JOYSTICK_PORTS];
  private static final Gamepad[] m_gamepads = new Gamepad[DriverStationBackend.JOYSTICK_PORTS];

  private static GenericHID getGenericHIDUnderLock(int port) {
    GenericHID toRet = m_hids[port];
    if (toRet == null) {
      toRet = new GenericHID(port);
      m_hids[port] = toRet;
    }
    return toRet;
  }

  /**
   * Gets the GenericHID object for the given port. GenericHID objects are cached, so this will
   * always return the same object for the same port.
   *
   * @param port The port index on the Driver Station that the controller is plugged into (0-5).
   * @return The GenericHID object for the given port.
   */
  public static GenericHID getGenericHID(int port) {
    m_dsLock.lock();
    try {
      return getGenericHIDUnderLock(port);
    } finally {
      m_dsLock.unlock();
    }
  }

  /**
   * Gets the Gamepad object for the given port. Gamepad objects are cached, so this will always
   * return the same object for the same port.
   *
   * @param port The port index on the Driver Station that the controller is plugged into (0-5).
   * @return The Gamepad object for the given port.
   */
  public static Gamepad getGamepad(int port) {
    m_dsLock.lock();
    try {
      Gamepad toRet = m_gamepads[port];
      if (toRet == null) {
        toRet = new Gamepad(getGenericHIDUnderLock(port));
        m_gamepads[port] = toRet;
      }
      return toRet;
    } finally {
      m_dsLock.unlock();
    }
  }

  /**
   * Starts logging DriverStation data to data log, including joystick data. Repeated calls are
   * ignored.
   *
   * @param log data log
   */
  public static void startDataLog(DataLog log) {
    DriverStationBackend.startDataLog(log);
  }

  /**
   * Starts logging DriverStation data to data log. Repeated calls are ignored.
   *
   * @param log data log
   * @param logJoysticks if true, log joystick data
   */
  public static void startDataLog(DataLog log, boolean logJoysticks) {
    DriverStationBackend.startDataLog(log, logJoysticks);
  }

  /**
   * Registers the given handle for DS data refresh notifications.
   *
   * @param handle The event handle.
   */
  public static void provideRefreshedDataEventHandle(int handle) {
    DriverStationBackend.provideRefreshedDataEventHandle(handle);
  }

  /**
   * Unregisters the given handle from DS data refresh notifications.
   *
   * @param handle The event handle.
   */
  public static void removeRefreshedDataEventHandle(int handle) {
    DriverStationBackend.removeRefreshedDataEventHandle(handle);
  }
}
