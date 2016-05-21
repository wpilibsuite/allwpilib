/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary;
import edu.wpi.first.wpilibj.communication.HALAllianceStationID;
import edu.wpi.first.wpilibj.communication.HALControlWord;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.hal.PowerJNI;

/*
 * Intentionally package private.
 */
/**
 * Provide access to the network communication data to / from the Driver Station.
 */
class HardwareDriverStation implements IDriverStation.WithPrivateMethods {

  private class HALJoystickButtons {
    public int m_buttons;
    public byte m_count;
  }

  private static final double JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL = 1.0;
  private double m_nextMessageTime = 0.0;

  private static class DriverStationTask implements Runnable {

    private HardwareDriverStation m_ds;

    DriverStationTask(HardwareDriverStation ds) {
      m_ds = ds;
    }

    public void run() {
      m_ds.task();
    }
  } /* DriverStationTask */

  private short[][] m_joystickAxes =
      new short[DriverStation.kJoystickPorts][FRCNetworkCommunicationsLibrary.kMaxJoystickAxes];
  private short[][] m_joystickPOVs =
      new short[DriverStation.kJoystickPorts][FRCNetworkCommunicationsLibrary.kMaxJoystickPOVs];
  private HALJoystickButtons[] m_joystickButtons =
      new HALJoystickButtons[DriverStation.kJoystickPorts];
  private int[] m_joystickIsXbox = new int[DriverStation.kJoystickPorts];
  private int[] m_joystickType = new int[DriverStation.kJoystickPorts];
  private String[] m_joystickName = new String[DriverStation.kJoystickPorts];
  private int[][] m_joystickAxisType =
      new int[DriverStation.kJoystickPorts][FRCNetworkCommunicationsLibrary.kMaxJoystickAxes];

  private Thread m_thread;
  private final Object m_dataSem;
  private volatile boolean m_threadKeepAlive = true;
  private boolean m_userInDisabled = false;
  private boolean m_userInAutonomous = false;
  private boolean m_userInTeleop = false;
  private boolean m_userInTest = false;
  private boolean m_newControlData;
  private final long m_packetDataAvailableMutex;
  private final long m_packetDataAvailableSem;

  /**
   * DriverStation constructor.
   *
   * <p>The single DriverStation instance is created statically with the instance static member
   * variable.
   */
  protected HardwareDriverStation() {
    m_dataSem = new Object();
    for (int i = 0; i < DriverStation.kJoystickPorts; i++) {
      m_joystickButtons[i] = new HALJoystickButtons();
    }

    m_packetDataAvailableMutex = HALUtil.initializeMutexNormal();
    m_packetDataAvailableSem = HALUtil.initializeMultiWait();
    FRCNetworkCommunicationsLibrary.setNewDataSem(m_packetDataAvailableSem);

    m_thread = new Thread(new DriverStationTask(this), "FRCDriverStation");
    m_thread.setPriority((Thread.NORM_PRIORITY + Thread.MAX_PRIORITY) / 2);

    m_thread.start();
  }

  /**
   * Kill the thread.
   */
  public void release() {
    m_threadKeepAlive = false;
  }

  /**
   * Provides the service routine for the DS polling m_thread.
   */
  private void task() {
    int safetyCounter = 0;
    while (m_threadKeepAlive) {
      HALUtil.takeMultiWait(m_packetDataAvailableSem, m_packetDataAvailableMutex);
      synchronized (this) {
        getData();
      }
      synchronized (m_dataSem) {
        m_dataSem.notifyAll();
      }
      if (++safetyCounter >= 4) {
        MotorSafetyHelper.checkMotors();
        safetyCounter = 0;
      }
      if (m_userInDisabled) {
        FRCNetworkCommunicationsLibrary.FRCNetworkCommunicationObserveUserProgramDisabled();
      }
      if (m_userInAutonomous) {
        FRCNetworkCommunicationsLibrary.FRCNetworkCommunicationObserveUserProgramAutonomous();
      }
      if (m_userInTeleop) {
        FRCNetworkCommunicationsLibrary.FRCNetworkCommunicationObserveUserProgramTeleop();
      }
      if (m_userInTest) {
        FRCNetworkCommunicationsLibrary.FRCNetworkCommunicationObserveUserProgramTest();
      }
    }
  }

  @Override
  public void waitForData(long timeout) {
    synchronized (m_dataSem) {
      try {
        m_dataSem.wait(timeout);
      } catch (InterruptedException ex) {
        Thread.currentThread().interrupt();
      }
    }
  }

  /**
   * Copy data from the DS task for the user. If no new data exists, it will just be returned,
   * otherwise the data will be copied from the DS polling loop.
   */
  protected synchronized void getData() {

    // Get the status of all of the joysticks
    for (byte stick = 0; stick < DriverStation.kJoystickPorts; stick++) {
      m_joystickAxes[stick] = FRCNetworkCommunicationsLibrary.HALGetJoystickAxes(stick);
      m_joystickPOVs[stick] = FRCNetworkCommunicationsLibrary.HALGetJoystickPOVs(stick);
      ByteBuffer countBuffer = ByteBuffer.allocateDirect(1);
      m_joystickButtons[stick].m_buttons =
          FRCNetworkCommunicationsLibrary.HALGetJoystickButtons(stick, countBuffer);
      m_joystickButtons[stick].m_count = countBuffer.get();
    }

    m_newControlData = true;
  }

  @Override
  public double getBatteryVoltage() {
    return PowerJNI.getVinVoltage();
  }

  /**
   * Reports errors related to unplugged joysticks Throttles the errors so that they don't overwhelm
   * the DS.
   */
  private void reportJoystickUnpluggedError(String message) {
    double currentTime = Timer.getFPGATimestamp();
    if (currentTime > m_nextMessageTime) {
      DriverStation.reportError(message, false);
      m_nextMessageTime = currentTime + JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL;
    }
  }

  /**
   * Reports errors related to unplugged joysticks Throttles the errors so that they don't overwhelm
   * the DS.
   */
  private void reportJoystickUnpluggedWarning(String message) {
    double currentTime = Timer.getFPGATimestamp();
    if (currentTime > m_nextMessageTime) {
      DriverStation.reportWarning(message, false);
      m_nextMessageTime = currentTime + JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL;
    }
  }

  @Override
  public synchronized double getStickAxis(int stick, int axis) {
    if (stick < 0 || stick >= DriverStation.kJoystickPorts) {
      throw new RuntimeException("Joystick index is out of range, should be 0-5");
    }

    if (axis < 0 || axis >= FRCNetworkCommunicationsLibrary.kMaxJoystickAxes) {
      throw new RuntimeException("Joystick axis is out of range");
    }

    if (axis >= m_joystickAxes[stick].length) {
      reportJoystickUnpluggedWarning("Joystick axis " + axis + " on port " + stick
          + " not available, check if controller is plugged in");
      return 0.0;
    }

    byte value = (byte) m_joystickAxes[stick][axis];

    if (value < 0) {
      return value / 128.0;
    } else {
      return value / 127.0;
    }
  }

  @Override
  public synchronized int getStickAxisCount(int stick) {

    if (stick < 0 || stick >= DriverStation.kJoystickPorts) {
      throw new RuntimeException("Joystick index is out of range, should be 0-5");
    }

    return m_joystickAxes[stick].length;
  }

  @Override
  public synchronized int getStickPOV(int stick, int pov) {
    if (stick < 0 || stick >= DriverStation.kJoystickPorts) {
      throw new RuntimeException("Joystick index is out of range, should be 0-5");
    }

    if (pov < 0 || pov >= FRCNetworkCommunicationsLibrary.kMaxJoystickPOVs) {
      throw new RuntimeException("Joystick POV is out of range");
    }

    if (pov >= m_joystickPOVs[stick].length) {
      reportJoystickUnpluggedWarning("Joystick POV " + pov + " on port " + stick
          + " not available, check if controller is plugged in");
      return -1;
    }

    return m_joystickPOVs[stick][pov];
  }

  @Override
  public synchronized int getStickPOVCount(int stick) {

    if (stick < 0 || stick >= DriverStation.kJoystickPorts) {
      throw new RuntimeException("Joystick index is out of range, should be 0-5");
    }

    return m_joystickPOVs[stick].length;
  }

  @Override
  public synchronized int getStickButtons(final int stick) {
    if (stick < 0 || stick >= DriverStation.kJoystickPorts) {
      throw new RuntimeException("Joystick index is out of range, should be 0-3");
    }

    return m_joystickButtons[stick].m_buttons;
  }

  @Override
  public synchronized boolean getStickButton(final int stick, byte button) {
    if (stick < 0 || stick >= DriverStation.kJoystickPorts) {
      throw new RuntimeException("Joystick index is out of range, should be 0-3");
    }


    if (button > m_joystickButtons[stick].m_count) {
      reportJoystickUnpluggedWarning("Joystick Button " + button + " on port " + stick
          + " not available, check if controller is plugged in");
      return false;
    }
    if (button <= 0) {
      reportJoystickUnpluggedError("Button indexes begin at 1 in WPILib for C++ and Java");
      return false;
    }
    return ((0x1 << (button - 1)) & m_joystickButtons[stick].m_buttons) != 0;
  }

  @Override
  public synchronized int getStickButtonCount(int stick) {
    if (stick < 0 || stick >= DriverStation.kJoystickPorts) {
      throw new RuntimeException("Joystick index is out of range, should be 0-5");
    }
    return m_joystickButtons[stick].m_count;
  }

  @Override
  public synchronized boolean getJoystickIsXbox(int stick) {

    if (stick < 0 || stick >= DriverStation.kJoystickPorts) {
      throw new RuntimeException("Joystick index is out of range, should be 0-5");
    }
    // TODO: Remove this when calling for descriptor on empty stick no longer
    // crashes
    if (1 > m_joystickButtons[stick].m_count && 1 > m_joystickAxes[stick].length) {
      reportJoystickUnpluggedWarning("Joystick on port " + stick
          + " not available, check if controller is plugged in");
      return false;
    }
    boolean retVal = false;
    if (FRCNetworkCommunicationsLibrary.HALGetJoystickIsXbox((byte) stick) == 1) {
      retVal = true;
    }
    return retVal;
  }

  @Override
  public synchronized int getJoystickType(int stick) {
    if (stick < 0 || stick >= DriverStation.kJoystickPorts) {
      throw new RuntimeException("Joystick index is out of range, should be 0-5");
    }
    // TODO: Remove this when calling for descriptor on empty stick no longer
    // crashes
    if (1 > m_joystickButtons[stick].m_count && 1 > m_joystickAxes[stick].length) {
      reportJoystickUnpluggedWarning("Joystick on port " + stick
          + " not available, check if controller is plugged in");
      return -1;
    }
    return FRCNetworkCommunicationsLibrary.HALGetJoystickType((byte) stick);
  }

  @Override
  public synchronized String getJoystickName(int stick) {

    if (stick < 0 || stick >= DriverStation.kJoystickPorts) {
      throw new RuntimeException("Joystick index is out of range, should be 0-5");
    }
    // TODO: Remove this when calling for descriptor on empty stick no longer
    // crashes
    if (1 > m_joystickButtons[stick].m_count && 1 > m_joystickAxes[stick].length) {
      reportJoystickUnpluggedWarning("Joystick on port " + stick
          + " not available, check if controller is plugged in");
      return "";
    }
    return FRCNetworkCommunicationsLibrary.HALGetJoystickName((byte) stick);
  }

  @Override
  public boolean isEnabled() {
    HALControlWord controlWord = FRCNetworkCommunicationsLibrary.HALGetControlWord();
    return controlWord.getEnabled() && controlWord.getDSAttached();
  }

  @Override
  public boolean isDisabled() {
    return !isEnabled();
  }

  @Override
  public boolean isAutonomous() {
    HALControlWord controlWord = FRCNetworkCommunicationsLibrary.HALGetControlWord();
    return controlWord.getAutonomous();
  }

  @Override
  public boolean isTest() {
    HALControlWord controlWord = FRCNetworkCommunicationsLibrary.HALGetControlWord();
    return controlWord.getTest();
  }

  @Override
  public boolean isOperatorControl() {
    return !(isAutonomous() || isTest());
  }

  @Override
  public boolean isSysActive() {
    return FRCNetworkCommunicationsLibrary.HALGetSystemActive();
  }

  @Override
  public boolean isBrownedOut() {
    return FRCNetworkCommunicationsLibrary.HALGetBrownedOut();
  }

  @Override
  public synchronized boolean isNewControlData() {
    boolean result = m_newControlData;
    m_newControlData = false;
    return result;
  }

  @Override
  public DriverStation.Alliance getAlliance() {
    HALAllianceStationID allianceStationID =
        FRCNetworkCommunicationsLibrary.HALGetAllianceStation();
    if (allianceStationID == null) {
      return DriverStation.Alliance.Invalid;
    }

    switch (allianceStationID) {
      case Red1:
      case Red2:
      case Red3:
        return DriverStation.Alliance.Red;

      case Blue1:
      case Blue2:
      case Blue3:
        return DriverStation.Alliance.Blue;

      default:
        return DriverStation.Alliance.Invalid;
    }
  }

  @Override
  public int getLocation() {
    HALAllianceStationID allianceStationID =
        FRCNetworkCommunicationsLibrary.HALGetAllianceStation();
    if (allianceStationID == null) {
      return 0;
    }
    switch (allianceStationID) {
      case Red1:
      case Blue1:
        return 1;

      case Red2:
      case Blue2:
        return 2;

      case Blue3:
      case Red3:
        return 3;

      default:
        return 0;
    }
  }

  @Override
  public boolean isFMSAttached() {
    HALControlWord controlWord = FRCNetworkCommunicationsLibrary.HALGetControlWord();
    return controlWord.getFMSAttached();
  }

  @Override
  public boolean isDSAttached() {
    HALControlWord controlWord = FRCNetworkCommunicationsLibrary.HALGetControlWord();
    return controlWord.getDSAttached();
  }

  @Override
  public double getMatchTime() {
    return FRCNetworkCommunicationsLibrary.HALGetMatchTime();
  }

  @Override
  public void reportErrorImpl(boolean isError, int code, String error,
                              boolean printTrace, String locString, String traceString) {
    FRCNetworkCommunicationsLibrary.HALSendError(isError, code, false, error, locString,
        printTrace ? traceString : "", true);
  }

  @SuppressWarnings("MethodName")
  @Override
  public void InDisabled(boolean entering) {
    m_userInDisabled = entering;
  }

  @SuppressWarnings("MethodName")
  @Override
  public void InAutonomous(boolean entering) {
    m_userInAutonomous = entering;
  }

  @SuppressWarnings("MethodName")
  @Override
  public void InOperatorControl(boolean entering) {
    m_userInTeleop = entering;
  }

  @SuppressWarnings("MethodName")
  @Override
  public void InTest(boolean entering) {
    m_userInTest = entering;
  }
}
