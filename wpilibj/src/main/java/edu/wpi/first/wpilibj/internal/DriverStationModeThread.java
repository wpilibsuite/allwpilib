// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.internal;

import edu.wpi.first.hal.DriverStationJNI;
import edu.wpi.first.util.WPIUtilJNI;
import edu.wpi.first.wpilibj.DriverStation;
import java.util.concurrent.atomic.AtomicBoolean;

/*
 * For internal use only.
 */
public class DriverStationModeThread implements AutoCloseable {
  private final AtomicBoolean m_keepAlive = new AtomicBoolean();
  private final Thread m_thread;

  private boolean m_userInDisabled;
  private boolean m_userInAutonomous;
  private boolean m_userInTeleop;
  private boolean m_userInTest;

  /** Internal use only. */
  public DriverStationModeThread() {
    m_keepAlive.set(true);
    m_thread = new Thread(this::run, "DriverStationMode");
    m_thread.start();
  }

  private void run() {
    int handle = WPIUtilJNI.createEvent(true, false);
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
      if (m_userInDisabled) {
        DriverStationJNI.observeUserProgramDisabled();
      }
      if (m_userInAutonomous) {
        DriverStationJNI.observeUserProgramAutonomous();
      }
      if (m_userInTeleop) {
        DriverStationJNI.observeUserProgramTeleop();
      }
      if (m_userInTest) {
        DriverStationJNI.observeUserProgramTest();
      }
    }

    DriverStationJNI.removeNewDataEventHandle(handle);
    WPIUtilJNI.destroyEvent(handle);
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting disabled code; if false, leaving disabled code
   */
  public void inDisabled(boolean entering) {
    m_userInDisabled = entering;
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting autonomous code; if false, leaving autonomous code
   */
  public void inAutonomous(boolean entering) {
    m_userInAutonomous = entering;
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting teleop code; if false, leaving teleop code
   */
  public void inTeleop(boolean entering) {
    m_userInTeleop = entering;
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting test code; if false, leaving test code
   */
  public void inTest(boolean entering) {
    m_userInTest = entering;
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
