// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.internal;

import edu.wpi.first.hal.DriverStationJNI;
import edu.wpi.first.util.WPIUtilJNI;
import edu.wpi.first.wpilibj.DriverStation;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

/** For internal use only. */
public class DriverStationModeThread implements AutoCloseable {
  private final AtomicBoolean m_keepAlive = new AtomicBoolean();
  private final Thread m_thread;

  private final AtomicInteger m_userInOpMode = new AtomicInteger();

  /** Internal use only. */
  public DriverStationModeThread() {
    m_keepAlive.set(true);
    m_thread = new Thread(this::run, "DriverStationMode");
    m_thread.start();
  }

  private void run() {
    int handle = WPIUtilJNI.createEvent(false, false);
    DriverStationJNI.provideNewDataEventHandle(handle);

    while (m_keepAlive.get()) {
      try {
        WPIUtilJNI.waitForObjectTimeout(handle, 0.1);
      } catch (InterruptedException e) {
        DriverStationJNI.removeNewDataEventHandle(handle);
        WPIUtilJNI.destroyEvent(handle);
        Thread.currentThread().interrupt();
        return;
      }
      DriverStation.refreshData();
      DriverStationJNI.observeUserProgramOpMode(m_userInOpMode.get());
    }

    DriverStationJNI.removeNewDataEventHandle(handle);
    WPIUtilJNI.destroyEvent(handle);
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param mode operating mode as provided by getOpModeId()
   */
  public void inOpMode(int mode) {
    m_userInOpMode.set(mode);
  }

  @Override
  public void close() {
    m_keepAlive.set(false);
    try {
      m_thread.join();
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
    }
  }
}
