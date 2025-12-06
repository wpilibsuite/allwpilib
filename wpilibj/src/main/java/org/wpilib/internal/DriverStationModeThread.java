// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.internal;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;
import org.wpilib.driverstation.DriverStation;
import org.wpilib.hardware.hal.ControlWord;
import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.util.WPIUtilJNI;

/** For internal use only. */
public class DriverStationModeThread implements AutoCloseable {
  private final AtomicBoolean m_keepAlive = new AtomicBoolean(true);
  private final AtomicLong m_userControlWord;
  private final int m_handle = WPIUtilJNI.createEvent(false, false);
  private final Thread m_thread;

  /**
   * Internal use only.
   *
   * @param word control word
   */
  public DriverStationModeThread(ControlWord word) {
    m_userControlWord = new AtomicLong(word.getNative());
    DriverStationJNI.provideNewDataEventHandle(m_handle);
    m_thread = new Thread(this::run, "DriverStationMode");
    m_thread.start();
  }

  private void run() {
    while (true) {
      try {
        WPIUtilJNI.waitForObjectTimeout(m_handle, 0.1);
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
        return;
      }
      if (!m_keepAlive.get()) {
        return;
      }
      DriverStation.refreshData();
      DriverStationJNI.observeUserProgram(m_userControlWord.get());
    }
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param word control word
   */
  public void inControl(ControlWord word) {
    m_userControlWord.set(word.getNative());
  }

  @Override
  public void close() {
    DriverStationJNI.removeNewDataEventHandle(m_handle);
    WPIUtilJNI.destroyEvent(m_handle);
    m_keepAlive.set(false);
    try {
      m_thread.join();
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
    }
  }
}
