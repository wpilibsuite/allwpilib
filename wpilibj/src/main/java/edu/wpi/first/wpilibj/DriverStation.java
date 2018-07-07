/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.hal.AllianceStationID;
import edu.wpi.first.wpilibj.hal.ControlWord;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.hal.MatchInfoData;
import edu.wpi.first.wpilibj.hal.PowerJNI;

/**
 * Provide access to the network communication data to / from the Driver Station.
 */
@SuppressWarnings({"PMD.CyclomaticComplexity", "PMD.ExcessiveClassLength",
                   "PMD.ExcessivePublicCount", "PMD.GodClass", "PMD.TooManyFields",
                   "PMD.TooManyMethods"})
public class DriverStation {
  /**
   * Number of Joystick Ports.
   */
  public static final int kJoystickPorts = 6;

  private class HALJoystickButtons {
    public int m_buttons;
    public byte m_count;
  }

  private class HALJoystickAxes {
    public float[] m_axes;
    public short m_count;

    HALJoystickAxes(int count) {
      m_axes = new float[count];
    }
  }

  private class HALJoystickPOVs {
    public short[] m_povs;
    public short m_count;

    HALJoystickPOVs(int count) {
      m_povs = new short[count];
    }
  }

  /**
   * The robot alliance that the robot is a part of.
   */
  public enum Alliance {
    Red, Blue, Invalid
  }

  public enum MatchType {
    None, Practice, Qualification, Elimination
  }

  private static final double JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL = 1.0;
  private double m_nextMessageTime;

  private static class DriverStationTask implements Runnable {
    private final DriverStation m_ds;

    DriverStationTask(DriverStation ds) {
      m_ds = ds;
    }

    @Override
    public void run() {
      m_ds.run();
    }
  } /* DriverStationTask */

  private static class MatchDataSender {
    @SuppressWarnings("MemberName")
    NetworkTable table;
    @SuppressWarnings("MemberName")
    NetworkTableEntry typeMetadata;
    @SuppressWarnings("MemberName")
    NetworkTableEntry gameSpecificMessage;
    @SuppressWarnings("MemberName")
    NetworkTableEntry eventName;
    @SuppressWarnings("MemberName")
    NetworkTableEntry matchNumber;
    @SuppressWarnings("MemberName")
    NetworkTableEntry replayNumber;
    @SuppressWarnings("MemberName")
    NetworkTableEntry matchType;
    @SuppressWarnings("MemberName")
    NetworkTableEntry alliance;
    @SuppressWarnings("MemberName")
    NetworkTableEntry station;
    @SuppressWarnings("MemberName")
    NetworkTableEntry controlWord;

    MatchDataSender() {
      table = NetworkTableInstance.getDefault().getTable("FMSInfo");
      typeMetadata = table.getEntry(".type");
      typeMetadata.forceSetString("FMSInfo");
      gameSpecificMessage = table.getEntry("GameSpecificMessage");
      gameSpecificMessage.forceSetString("");
      eventName = table.getEntry("EventName");
      eventName.forceSetString("");
      matchNumber = table.getEntry("MatchNumber");
      matchNumber.forceSetDouble(0);
      replayNumber = table.getEntry("ReplayNumber");
      replayNumber.forceSetDouble(0);
      matchType = table.getEntry("MatchType");
      matchType.forceSetDouble(0);
      alliance = table.getEntry("IsRedAlliance");
      alliance.forceSetBoolean(true);
      station = table.getEntry("StationNumber");
      station.forceSetDouble(1);
      controlWord = table.getEntry("FMSControlData");
      controlWord.forceSetDouble(0);
    }
  }

  private static DriverStation instance = new DriverStation();

  // Joystick User Data
  private HALJoystickAxes[] m_joystickAxes = new HALJoystickAxes[kJoystickPorts];
  private HALJoystickPOVs[] m_joystickPOVs = new HALJoystickPOVs[kJoystickPorts];
  private HALJoystickButtons[] m_joystickButtons = new HALJoystickButtons[kJoystickPorts];
  private MatchInfoData m_matchInfo = new MatchInfoData();

  // Joystick Cached Data
  private HALJoystickAxes[] m_joystickAxesCache = new HALJoystickAxes[kJoystickPorts];
  private HALJoystickPOVs[] m_joystickPOVsCache = new HALJoystickPOVs[kJoystickPorts];
  private HALJoystickButtons[] m_joystickButtonsCache = new HALJoystickButtons[kJoystickPorts];
  private MatchInfoData m_matchInfoCache = new MatchInfoData();

  // Joystick button rising/falling edge flags
  private int[] m_joystickButtonsPressed = new int[kJoystickPorts];
  private int[] m_joystickButtonsReleased = new int[kJoystickPorts];

  // preallocated byte buffer for button count
  private final ByteBuffer m_buttonCountBuffer = ByteBuffer.allocateDirect(1);

  private final MatchDataSender m_matchDataSender;

  // Internal Driver Station thread
  @SuppressWarnings("PMD.SingularField")
  private final Thread m_thread;
  private volatile boolean m_threadKeepAlive = true;

  private final ReentrantLock m_cacheDataMutex = new ReentrantLock();

  private final Lock m_waitForDataMutex;
  private final Condition m_waitForDataCond;
  private int m_waitForDataCount;

  // Robot state status variables
  private boolean m_userInDisabled;
  private boolean m_userInAutonomous;
  private boolean m_userInTeleop;
  private boolean m_userInTest;

  // Control word variables
  private final Object m_controlWordMutex;
  private final ControlWord m_controlWordCache;
  private long m_lastControlWordUpdate;

  /**
   * Gets an instance of the DriverStation.
   *
   * @return The DriverStation.
   */
  public static DriverStation getInstance() {
    return DriverStation.instance;
  }

  /**
   * DriverStation constructor.
   *
   * <p>The single DriverStation instance is created statically with the instance static member
   * variable.
   */
  @SuppressWarnings("PMD.AvoidInstantiatingObjectsInLoops")
  private DriverStation() {
    HAL.initialize(500, 0);
    m_waitForDataCount = 0;
    m_waitForDataMutex = new ReentrantLock();
    m_waitForDataCond = m_waitForDataMutex.newCondition();

    for (int i = 0; i < kJoystickPorts; i++) {
      m_joystickButtons[i] = new HALJoystickButtons();
      m_joystickAxes[i] = new HALJoystickAxes(HAL.kMaxJoystickAxes);
      m_joystickPOVs[i] = new HALJoystickPOVs(HAL.kMaxJoystickPOVs);

      m_joystickButtonsCache[i] = new HALJoystickButtons();
      m_joystickAxesCache[i] = new HALJoystickAxes(HAL.kMaxJoystickAxes);
      m_joystickPOVsCache[i] = new HALJoystickPOVs(HAL.kMaxJoystickPOVs);
    }

    m_controlWordMutex = new Object();
    m_controlWordCache = new ControlWord();
    m_lastControlWordUpdate = 0;

    m_matchDataSender = new MatchDataSender();

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
   * Report error to Driver Station. Optionally appends Stack trace
   * to error message.
   *
   * @param printTrace If true, append stack trace to error string
   */
  public static void reportError(String error, boolean printTrace) {
    reportErrorImpl(true, 1, error, printTrace);
  }

  /**
   * Report error to Driver Station. Appends provided stack trace
   * to error message.
   *
   * @param stackTrace The stack trace to append
   */
  public static void reportError(String error, StackTraceElement[] stackTrace) {
    reportErrorImpl(true, 1, error, stackTrace);
  }

  /**
   * Report warning to Driver Station. Optionally appends Stack
   * trace to warning message.
   *
   * @param printTrace If true, append stack trace to warning string
   */
  public static void reportWarning(String error, boolean printTrace) {
    reportErrorImpl(false, 1, error, printTrace);
  }

  /**
   * Report warning to Driver Station. Appends provided stack
   * trace to warning message.
   *
   * @param stackTrace The stack trace to append
   */
  public static void reportWarning(String error, StackTraceElement[] stackTrace) {
    reportErrorImpl(false, 1, error, stackTrace);
  }

  private static void reportErrorImpl(boolean isError, int code, String error, boolean
      printTrace) {
    reportErrorImpl(isError, code, error, printTrace, Thread.currentThread().getStackTrace(), 3);
  }

  private static void reportErrorImpl(boolean isError, int code, String error,
      StackTraceElement[] stackTrace) {
    reportErrorImpl(isError, code, error, true, stackTrace, 0);
  }

  private static void reportErrorImpl(boolean isError, int code, String error,
      boolean printTrace, StackTraceElement[] stackTrace, int stackTraceFirst) {
    String locString;
    if (stackTrace.length >= stackTraceFirst + 1) {
      locString = stackTrace[stackTraceFirst].toString();
    } else {
      locString = "";
    }
    StringBuilder traceString = new StringBuilder("");
    if (printTrace) {
      boolean haveLoc = false;
      for (int i = stackTraceFirst; i < stackTrace.length; i++) {
        String loc = stackTrace[i].toString();
        traceString.append("\tat ").append(loc).append('\n');
        // get first user function
        if (!haveLoc && !loc.startsWith("edu.wpi.first")) {
          locString = loc;
          haveLoc = true;
        }
      }
    }
    HAL.sendError(isError, code, false, error, locString, traceString.toString(), true);
  }

  /**
   * The state of one joystick button. Button indexes begin at 1.
   *
   * @param stick  The joystick to read.
   * @param button The button index, beginning at 1.
   * @return The state of the joystick button.
   */
  public boolean getStickButton(final int stick, final int button) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-3");
    }
    if (button <= 0) {
      reportJoystickUnpluggedError("Button indexes begin at 1 in WPILib for C++ and Java\n");
      return false;
    }
    m_cacheDataMutex.lock();
    try {
      if (button > m_joystickButtons[stick].m_count) {
        // Unlock early so error printing isn't locked.
        m_cacheDataMutex.unlock();
        reportJoystickUnpluggedWarning("Joystick Button " + button + " on port " + stick
            + " not available, check if controller is plugged in");
      }

      return (m_joystickButtons[stick].m_buttons & 1 << (button - 1)) != 0;
    } finally {
      if (m_cacheDataMutex.isHeldByCurrentThread()) {
        m_cacheDataMutex.unlock();
      }
    }
  }

  /**
   * Whether one joystick button was pressed since the last check. Button indexes begin at 1.
   *
   * @param stick  The joystick to read.
   * @param button The button index, beginning at 1.
   * @return Whether the joystick button was pressed since the last check.
   */
  boolean getStickButtonPressed(final int stick, final int button) {
    if (button <= 0) {
      reportJoystickUnpluggedError("Button indexes begin at 1 in WPILib for C++ and Java\n");
      return false;
    }
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-3");
    }
    boolean error = false;
    boolean retVal = false;
    synchronized (m_cacheDataMutex) {
      if (button > m_joystickButtons[stick].m_count) {
        error = true;
        retVal = false;
      } else {
        // If button was pressed, clear flag and return true
        if ((m_joystickButtonsPressed[stick] & 1 << (button - 1)) != 0) {
          m_joystickButtonsPressed[stick] &= ~(1 << (button - 1));
          retVal = true;
        } else {
          retVal = false;
        }
      }
    }
    if (error) {
      reportJoystickUnpluggedWarning("Joystick Button " + button + " on port " + stick
          + " not available, check if controller is plugged in");
    }
    return retVal;
  }

  /**
   * Whether one joystick button was released since the last check. Button indexes
   * begin at 1.
   *
   * @param stick  The joystick to read.
   * @param button The button index, beginning at 1.
   * @return Whether the joystick button was released since the last check.
   */
  boolean getStickButtonReleased(final int stick, final int button) {
    if (button <= 0) {
      reportJoystickUnpluggedError("Button indexes begin at 1 in WPILib for C++ and Java\n");
      return false;
    }
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-3");
    }
    boolean error = false;
    boolean retVal = false;
    synchronized (m_cacheDataMutex) {
      if (button > m_joystickButtons[stick].m_count) {
        error = true;
        retVal = false;
      } else {
        // If button was released, clear flag and return true
        if ((m_joystickButtonsReleased[stick] & 1 << (button - 1)) != 0) {
          m_joystickButtonsReleased[stick] &= ~(1 << (button - 1));
          retVal = true;
        } else {
          retVal = false;
        }
      }
    }
    if (error) {
      reportJoystickUnpluggedWarning("Joystick Button " + button + " on port " + stick
          + " not available, check if controller is plugged in");
    }
    return retVal;
  }

  /**
   * Get the value of the axis on a joystick. This depends on the mapping of the joystick connected
   * to the specified port.
   *
   * @param stick The joystick to read.
   * @param axis  The analog axis value to read from the joystick.
   * @return The value of the axis on the joystick.
   */
  public double getStickAxis(int stick, int axis) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }
    if (axis >= HAL.kMaxJoystickAxes) {
      throw new IllegalArgumentException("Joystick axis is out of range");
    }

    m_cacheDataMutex.lock();
    try {
      if (axis >= m_joystickAxes[stick].m_count) {
        // Unlock early so error printing isn't locked.
        m_cacheDataMutex.unlock();
        reportJoystickUnpluggedWarning("Joystick axis " + axis + " on port " + stick
            + " not available, check if controller is plugged in");
        return 0.0;
      }

      return m_joystickAxes[stick].m_axes[axis];
    } finally {
      if (m_cacheDataMutex.isHeldByCurrentThread()) {
        m_cacheDataMutex.unlock();
      }
    }
  }

  /**
   * Get the state of a POV on the joystick.
   *
   * @return the angle of the POV in degrees, or -1 if the POV is not pressed.
   */
  public int getStickPOV(int stick, int pov) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }
    if (pov >= HAL.kMaxJoystickPOVs) {
      throw new IllegalArgumentException("Joystick POV is out of range");
    }

    m_cacheDataMutex.lock();
    try {
      if (pov >= m_joystickPOVs[stick].m_count) {
        // Unlock early so error printing isn't locked.
        m_cacheDataMutex.unlock();
        reportJoystickUnpluggedWarning("Joystick POV " + pov + " on port " + stick
            + " not available, check if controller is plugged in");
      }
    } finally {
      if (m_cacheDataMutex.isHeldByCurrentThread()) {
        m_cacheDataMutex.unlock();
      }
    }

    return m_joystickPOVs[stick].m_povs[pov];
  }

  /**
   * The state of the buttons on the joystick.
   *
   * @param stick The joystick to read.
   * @return The state of the buttons on the joystick.
   */
  public int getStickButtons(final int stick) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-3");
    }

    m_cacheDataMutex.lock();
    try {
      return m_joystickButtons[stick].m_buttons;
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Returns the number of axes on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The number of axes on the indicated joystick
   */
  public int getStickAxisCount(int stick) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    m_cacheDataMutex.lock();
    try {
      return m_joystickAxes[stick].m_count;
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Returns the number of POVs on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The number of POVs on the indicated joystick
   */
  public int getStickPOVCount(int stick) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    m_cacheDataMutex.lock();
    try {
      return m_joystickPOVs[stick].m_count;
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets the number of buttons on a joystick.
   *
   * @param stick The joystick port number
   * @return The number of buttons on the indicated joystick
   */
  public int getStickButtonCount(int stick) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    m_cacheDataMutex.lock();
    try {
      return m_joystickButtons[stick].m_count;
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets the value of isXbox on a joystick.
   *
   * @param stick The joystick port number
   * @return A boolean that returns the value of isXbox
   */
  public boolean getJoystickIsXbox(int stick) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    m_cacheDataMutex.lock();
    try {
      return HAL.getJoystickIsXbox((byte) stick) == 1;
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets the value of type on a joystick.
   *
   * @param stick The joystick port number
   * @return The value of type
   */
  public int getJoystickType(int stick) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    m_cacheDataMutex.lock();
    try {
      return HAL.getJoystickType((byte) stick);
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets the name of the joystick at a port.
   *
   * @param stick The joystick port number
   * @return The value of name
   */
  public String getJoystickName(int stick) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    m_cacheDataMutex.lock();
    try {
      return HAL.getJoystickName((byte) stick);
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Returns the types of Axes on a given joystick port.
   *
   * @param stick The joystick port number
   * @param axis The target axis
   * @return What type of axis the axis is reporting to be
   */
  public int getJoystickAxisType(int stick, int axis) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    m_cacheDataMutex.lock();
    try {
      return HAL.getJoystickAxisType((byte) stick, (byte) axis);
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be enabled.
   *
   * @return True if the robot is enabled, false otherwise.
   */
  public boolean isEnabled() {
    synchronized (m_controlWordMutex) {
      updateControlWord(false);
      return m_controlWordCache.getEnabled() && m_controlWordCache.getDSAttached();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be disabled.
   *
   * @return True if the robot should be disabled, false otherwise.
   */
  public boolean isDisabled() {
    return !isEnabled();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode.
   *
   * @return True if autonomous mode should be enabled, false otherwise.
   */
  public boolean isAutonomous() {
    synchronized (m_controlWordMutex) {
      updateControlWord(false);
      return m_controlWordCache.getAutonomous();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controlled mode.
   *
   * @return True if operator-controlled mode should be enabled, false otherwise.
   */
  public boolean isOperatorControl() {
    return !(isAutonomous() || isTest());
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in test
   * mode.
   *
   * @return True if test mode should be enabled, false otherwise.
   */
  public boolean isTest() {
    synchronized (m_controlWordMutex) {
      updateControlWord(false);
      return m_controlWordCache.getTest();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station is attached.
   *
   * @return True if Driver Station is attached, false otherwise.
   */
  public boolean isDSAttached() {
    synchronized (m_controlWordMutex) {
      updateControlWord(false);
      return m_controlWordCache.getDSAttached();
    }
  }

  /**
   * Gets if a new control packet from the driver station arrived since the last time this function
   * was called.
   *
   * @return True if the control data has been updated since the last call.
   */
  public boolean isNewControlData() {
    return HAL.isNewControlData();
  }

  /**
   * Gets if the driver station attached to a Field Management System.
   *
   * @return true if the robot is competing on a field being controlled by a Field Management System
   */
  public boolean isFMSAttached() {
    synchronized (m_controlWordMutex) {
      updateControlWord(false);
      return m_controlWordCache.getFMSAttached();
    }
  }

  /**
   * Gets a value indicating whether the FPGA outputs are enabled. The outputs may be disabled if
   * the robot is disabled or e-stopped, the watchdog has expired, or if the roboRIO browns out.
   *
   * @return True if the FPGA outputs are enabled.
   * @deprecated Use RobotController.isSysActive()
   */
  @Deprecated
  public boolean isSysActive() {
    return HAL.getSystemActive();
  }

  /**
   * Check if the system is browned out.
   *
   * @return True if the system is browned out
   * @deprecated Use RobotController.isBrownedOut()
   */
  @Deprecated
  public boolean isBrownedOut() {
    return HAL.getBrownedOut();
  }

  /**
   * Get the game specific message.
   *
   * @return the game specific message
   */
  public String getGameSpecificMessage() {
    m_cacheDataMutex.lock();
    try {
      return m_matchInfo.gameSpecificMessage;
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Get the event name.
   *
   * @return the event name
   */
  public String getEventName() {
    m_cacheDataMutex.lock();
    try {
      return m_matchInfo.eventName;
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Get the match type.
   *
   * @return the match type
   */
  public MatchType getMatchType() {
    int matchType;
    m_cacheDataMutex.lock();
    try {
      matchType = m_matchInfo.matchType;
    } finally {
      m_cacheDataMutex.unlock();
    }
    switch (matchType) {
      case 1:
        return MatchType.Practice;
      case 2:
        return MatchType.Qualification;
      case 3:
        return MatchType.Elimination;
      default:
        return MatchType.None;
    }
  }

  /**
   * Get the match number.
   *
   * @return the match number
   */
  public int getMatchNumber() {
    m_cacheDataMutex.lock();
    try {
      return m_matchInfo.matchNumber;
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Get the replay number.
   *
   * @return the replay number
   */
  public int getReplayNumber() {
    m_cacheDataMutex.lock();
    try {
      return m_matchInfo.replayNumber;
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Get the current alliance from the FMS.
   *
   * @return the current alliance
   */
  public Alliance getAlliance() {
    AllianceStationID allianceStationID = HAL.getAllianceStation();
    if (allianceStationID == null) {
      return Alliance.Invalid;
    }

    switch (allianceStationID) {
      case Red1:
      case Red2:
      case Red3:
        return Alliance.Red;

      case Blue1:
      case Blue2:
      case Blue3:
        return Alliance.Blue;

      default:
        return Alliance.Invalid;
    }
  }

  /**
   * Gets the location of the team's driver station controls.
   *
   * @return the location of the team's driver station controls: 1, 2, or 3
   */
  public int getLocation() {
    AllianceStationID allianceStationID = HAL.getAllianceStation();
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

  /**
   * Wait for new data from the driver station.
   */
  public void waitForData() {
    waitForData(0);
  }

  /**
   * Wait for new data or for timeout, which ever comes first. If timeout is 0, wait for new data
   * only.
   *
   * @param timeout The maximum time in seconds to wait.
   * @return true if there is new data, otherwise false
   */
  public boolean waitForData(double timeout) {
    long startTime = RobotController.getFPGATime();
    long timeoutMicros = (long) (timeout * 1000000);
    m_waitForDataMutex.lock();
    try {
      int currentCount = m_waitForDataCount;
      while (m_waitForDataCount == currentCount) {
        if (timeout > 0) {
          long now = RobotController.getFPGATime();
          if (now < startTime + timeoutMicros) {
            // We still have time to wait
            boolean signaled = m_waitForDataCond.await(startTime + timeoutMicros - now,
                                                TimeUnit.MICROSECONDS);
            if (!signaled) {
              // Return false if a timeout happened
              return false;
            }
          } else {
            // Time has elapsed.
            return false;
          }
        } else {
          m_waitForDataCond.await();
        }
      }
      // Return true if we have received a proper signal
      return true;
    } catch (InterruptedException ex) {
      // return false on a thread interrupt
      return false;
    } finally {
      m_waitForDataMutex.unlock();
    }
  }

  /**
   * Return the approximate match time. The FMS does not send an official match time to the robots,
   * but does send an approximate match time. The value will count down the time remaining in the
   * current period (auto or teleop). Warning: This is not an official time (so it cannot be used to
   * dispute ref calls or guarantee that a function will trigger before the match ends) The
   * Practice Match function of the DS approximates the behaviour seen on the field.
   *
   * @return Time remaining in current match period (auto or teleop) in seconds
   */
  public double getMatchTime() {
    return HAL.getMatchTime();
  }

  /**
   * Read the battery voltage.
   *
   * @return The battery voltage in Volts.
   * @deprecated Use RobotController.getBatteryVoltage
   */
  @Deprecated
  public double getBatteryVoltage() {
    return PowerJNI.getVinVoltage();
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting disabled code; if false, leaving disabled code
   */
  @SuppressWarnings("MethodName")
  public void InDisabled(boolean entering) {
    m_userInDisabled = entering;
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting autonomous code; if false, leaving autonomous code
   */
  @SuppressWarnings("MethodName")
  public void InAutonomous(boolean entering) {
    m_userInAutonomous = entering;
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting teleop code; if false, leaving teleop code
   */
  @SuppressWarnings("MethodName")
  public void InOperatorControl(boolean entering) {
    m_userInTeleop = entering;
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting test code; if false, leaving test code
   */
  @SuppressWarnings("MethodName")
  public void InTest(boolean entering) {
    m_userInTest = entering;
  }

  private void sendMatchData() {
    AllianceStationID alliance = HAL.getAllianceStation();
    boolean isRedAlliance = false;
    int stationNumber = 1;
    switch (alliance) {
      case Blue1:
        isRedAlliance = false;
        stationNumber = 1;
        break;
      case Blue2:
        isRedAlliance = false;
        stationNumber = 2;
        break;
      case Blue3:
        isRedAlliance = false;
        stationNumber = 3;
        break;
      case Red1:
        isRedAlliance = true;
        stationNumber = 1;
        break;
      case Red2:
        isRedAlliance = true;
        stationNumber = 2;
        break;
      default:
        isRedAlliance = true;
        stationNumber = 3;
        break;
    }


    String eventName;
    String gameSpecificMessage;
    int matchNumber;
    int replayNumber;
    int matchType;
    synchronized (m_cacheDataMutex) {
      eventName = m_matchInfo.eventName;
      gameSpecificMessage = m_matchInfo.gameSpecificMessage;
      matchNumber = m_matchInfo.matchNumber;
      replayNumber = m_matchInfo.replayNumber;
      matchType = m_matchInfo.matchType;
    }

    m_matchDataSender.alliance.setBoolean(isRedAlliance);
    m_matchDataSender.station.setDouble(stationNumber);
    m_matchDataSender.eventName.setString(eventName);
    m_matchDataSender.gameSpecificMessage.setString(gameSpecificMessage);
    m_matchDataSender.matchNumber.setDouble(matchNumber);
    m_matchDataSender.replayNumber.setDouble(replayNumber);
    m_matchDataSender.matchType.setDouble(matchType);
    m_matchDataSender.controlWord.setDouble(HAL.nativeGetControlWord());
  }

  /**
   * Copy data from the DS task for the user. If no new data exists, it will just be returned,
   * otherwise the data will be copied from the DS polling loop.
   */
  protected void getData() {
    // Get the status of all of the joysticks
    for (byte stick = 0; stick < kJoystickPorts; stick++) {
      m_joystickAxesCache[stick].m_count =
          HAL.getJoystickAxes(stick, m_joystickAxesCache[stick].m_axes);
      m_joystickPOVsCache[stick].m_count =
          HAL.getJoystickPOVs(stick, m_joystickPOVsCache[stick].m_povs);
      m_joystickButtonsCache[stick].m_buttons = HAL.getJoystickButtons(stick, m_buttonCountBuffer);
      m_joystickButtonsCache[stick].m_count = m_buttonCountBuffer.get(0);
    }

    HAL.getMatchInfo(m_matchInfoCache);

    // Force a control word update, to make sure the data is the newest.
    updateControlWord(true);

    // lock joystick mutex to swap cache data
    m_cacheDataMutex.lock();
    try {
      for (int i = 0; i < kJoystickPorts; i++) {
        // If buttons weren't pressed and are now, set flags in m_buttonsPressed
        m_joystickButtonsPressed[i] |=
            ~m_joystickButtons[i].m_buttons & m_joystickButtonsCache[i].m_buttons;

        // If buttons were pressed and aren't now, set flags in m_buttonsReleased
        m_joystickButtonsReleased[i] |=
            m_joystickButtons[i].m_buttons & ~m_joystickButtonsCache[i].m_buttons;
      }

      // move cache to actual data
      HALJoystickAxes[] currentAxes = m_joystickAxes;
      m_joystickAxes = m_joystickAxesCache;
      m_joystickAxesCache = currentAxes;

      HALJoystickButtons[] currentButtons = m_joystickButtons;
      m_joystickButtons = m_joystickButtonsCache;
      m_joystickButtonsCache = currentButtons;

      HALJoystickPOVs[] currentPOVs = m_joystickPOVs;
      m_joystickPOVs = m_joystickPOVsCache;
      m_joystickPOVsCache = currentPOVs;

      MatchInfoData currentInfo = m_matchInfo;
      m_matchInfo = m_matchInfoCache;
      m_matchInfoCache = currentInfo;
    } finally {
      m_cacheDataMutex.unlock();
    }

    m_waitForDataMutex.lock();
    m_waitForDataCount++;
    m_waitForDataCond.signalAll();
    m_waitForDataMutex.unlock();

    sendMatchData();
  }

  /**
   * Reports errors related to unplugged joysticks Throttles the errors so that they don't overwhelm
   * the DS.
   */
  private void reportJoystickUnpluggedError(String message) {
    double currentTime = Timer.getFPGATimestamp();
    if (currentTime > m_nextMessageTime) {
      reportError(message, false);
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
      reportWarning(message, false);
      m_nextMessageTime = currentTime + JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL;
    }
  }

  /**
   * Provides the service routine for the DS polling m_thread.
   */
  private void run() {
    int safetyCounter = 0;
    while (m_threadKeepAlive) {
      HAL.waitForDSData();
      getData();

      if (isDisabled()) {
        safetyCounter = 0;
      }

      safetyCounter++;
      if (safetyCounter >= 4) {
        MotorSafetyHelper.checkMotors();
        safetyCounter = 0;
      }
      if (m_userInDisabled) {
        HAL.observeUserProgramDisabled();
      }
      if (m_userInAutonomous) {
        HAL.observeUserProgramAutonomous();
      }
      if (m_userInTeleop) {
        HAL.observeUserProgramTeleop();
      }
      if (m_userInTest) {
        HAL.observeUserProgramTest();
      }
    }
  }

  /**
   * Updates the data in the control word cache. Updates if the force parameter is set, or if
   * 50ms have passed since the last update.
   *
   * @param force True to force an update to the cache, otherwise update if 50ms have passed.
   */
  private void updateControlWord(boolean force) {
    long now = System.currentTimeMillis();
    synchronized (m_controlWordMutex) {
      if (now - m_lastControlWordUpdate > 50 || force) {
        HAL.getControlWord(m_controlWordCache);
        m_lastControlWordUpdate = now;
      }
    }
  }
}
