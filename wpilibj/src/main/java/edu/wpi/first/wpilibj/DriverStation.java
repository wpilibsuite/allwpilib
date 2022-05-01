// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.AllianceStationID;
import edu.wpi.first.hal.ControlWord;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.MatchInfoData;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.util.WPIUtilJNI;
import edu.wpi.first.util.datalog.BooleanArrayLogEntry;
import edu.wpi.first.util.datalog.BooleanLogEntry;
import edu.wpi.first.util.datalog.DataLog;
import edu.wpi.first.util.datalog.FloatArrayLogEntry;
import edu.wpi.first.util.datalog.IntegerArrayLogEntry;
import java.nio.ByteBuffer;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/** Provide access to the network communication data to / from the Driver Station. */
public final class DriverStation {
  /** Number of Joystick Ports. */
  public static final int kJoystickPorts = 6;

  private static class HALJoystickButtons {
    public int m_buttons;
    public byte m_count;
  }

  private static class HALJoystickAxes {
    public float[] m_axes;
    public short m_count;

    HALJoystickAxes(int count) {
      m_axes = new float[count];
    }
  }

  private static class HALJoystickPOVs {
    public short[] m_povs;
    public short m_count;

    HALJoystickPOVs(int count) {
      m_povs = new short[count];
      for (int i = 0; i < count; i++) {
        m_povs[i] = -1;
      }
    }
  }

  /** The robot alliance that the robot is a part of. */
  public enum Alliance {
    Red,
    Blue,
    Invalid
  }

  public enum MatchType {
    None,
    Practice,
    Qualification,
    Elimination
  }

  private static final double JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL = 1.0;
  private static double m_nextMessageTime;

  private static class DriverStationTask implements Runnable {
    DriverStationTask() {}

    @Override
    public void run() {
      DriverStation.run();
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

    @SuppressWarnings("MemberName")
    boolean oldIsRedAlliance = true;

    @SuppressWarnings("MemberName")
    int oldStationNumber = 1;

    @SuppressWarnings("MemberName")
    String oldEventName = "";

    @SuppressWarnings("MemberName")
    String oldGameSpecificMessage = "";

    @SuppressWarnings("MemberName")
    int oldMatchNumber;

    @SuppressWarnings("MemberName")
    int oldReplayNumber;

    @SuppressWarnings("MemberName")
    int oldMatchType;

    @SuppressWarnings("MemberName")
    int oldControlWord;

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

    private void sendMatchData() {
      AllianceStationID allianceID = HAL.getAllianceStation();
      boolean isRedAlliance = false;
      int stationNumber = 1;
      switch (allianceID) {
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

      String currentEventName;
      String currentGameSpecificMessage;
      int currentMatchNumber;
      int currentReplayNumber;
      int currentMatchType;
      int currentControlWord;
      m_cacheDataMutex.lock();
      try {
        currentEventName = DriverStation.m_matchInfo.eventName;
        currentGameSpecificMessage = DriverStation.m_matchInfo.gameSpecificMessage;
        currentMatchNumber = DriverStation.m_matchInfo.matchNumber;
        currentReplayNumber = DriverStation.m_matchInfo.replayNumber;
        currentMatchType = DriverStation.m_matchInfo.matchType;
      } finally {
        m_cacheDataMutex.unlock();
      }
      currentControlWord = HAL.nativeGetControlWord();

      if (oldIsRedAlliance != isRedAlliance) {
        alliance.setBoolean(isRedAlliance);
        oldIsRedAlliance = isRedAlliance;
      }
      if (oldStationNumber != stationNumber) {
        station.setDouble(stationNumber);
        oldStationNumber = stationNumber;
      }
      if (!oldEventName.equals(currentEventName)) {
        eventName.setString(currentEventName);
        oldEventName = currentEventName;
      }
      if (!oldGameSpecificMessage.equals(currentGameSpecificMessage)) {
        gameSpecificMessage.setString(currentGameSpecificMessage);
        oldGameSpecificMessage = currentGameSpecificMessage;
      }
      if (currentMatchNumber != oldMatchNumber) {
        matchNumber.setDouble(currentMatchNumber);
        oldMatchNumber = currentMatchNumber;
      }
      if (currentReplayNumber != oldReplayNumber) {
        replayNumber.setDouble(currentReplayNumber);
        oldReplayNumber = currentReplayNumber;
      }
      if (currentMatchType != oldMatchType) {
        matchType.setDouble(currentMatchType);
        oldMatchType = currentMatchType;
      }
      if (currentControlWord != oldControlWord) {
        controlWord.setDouble(currentControlWord);
        oldControlWord = currentControlWord;
      }
    }
  }

  private static class JoystickLogSender {
    JoystickLogSender(DataLog log, int stick, long timestamp) {
      m_stick = stick;

      m_logButtons = new BooleanArrayLogEntry(log, "DS:joystick" + stick + "/buttons", timestamp);
      m_logAxes = new FloatArrayLogEntry(log, "DS:joystick" + stick + "/axes", timestamp);
      m_logPOVs = new IntegerArrayLogEntry(log, "DS:joystick" + stick + "/povs", timestamp);

      appendButtons(timestamp);
      appendAxes(timestamp);
      appendPOVs(timestamp);
    }

    public void send(long timestamp) {
      if (m_joystickButtonsCache[m_stick].m_count != m_joystickButtons[m_stick].m_count
          || m_joystickButtonsCache[m_stick].m_buttons != m_joystickButtons[m_stick].m_buttons) {
        appendButtons(timestamp);
      }

      if (m_joystickAxesCache[m_stick].m_count != m_joystickAxes[m_stick].m_count) {
        appendAxes(timestamp);
      } else {
        int count = m_joystickAxesCache[m_stick].m_count;
        for (int i = 0; i < count; i++) {
          if (m_joystickAxesCache[m_stick].m_axes[i] != m_joystickAxes[m_stick].m_axes[i]) {
            appendAxes(timestamp);
            break;
          }
        }
      }

      if (m_joystickPOVsCache[m_stick].m_count != m_joystickPOVs[m_stick].m_count) {
        appendPOVs(timestamp);
      } else {
        int count = m_joystickPOVsCache[m_stick].m_count;
        for (int i = 0; i < count; i++) {
          if (m_joystickPOVsCache[m_stick].m_povs[i] != m_joystickPOVs[m_stick].m_povs[i]) {
            appendPOVs(timestamp);
            break;
          }
        }
      }
    }

    void appendButtons(long timestamp) {
      int count = m_joystickButtonsCache[m_stick].m_count;
      if (m_sizedButtons == null || m_sizedButtons.length != count) {
        m_sizedButtons = new boolean[count];
      }
      int buttons = m_joystickButtonsCache[m_stick].m_buttons;
      for (int i = 0; i < count; i++) {
        m_sizedButtons[i] = (buttons & (1 << i)) != 0;
      }
      m_logButtons.append(m_sizedButtons, timestamp);
    }

    void appendAxes(long timestamp) {
      int count = m_joystickAxesCache[m_stick].m_count;
      if (m_sizedAxes == null || m_sizedAxes.length != count) {
        m_sizedAxes = new float[count];
      }
      System.arraycopy(m_joystickAxesCache[m_stick].m_axes, 0, m_sizedAxes, 0, count);
      m_logAxes.append(m_sizedAxes, timestamp);
    }

    void appendPOVs(long timestamp) {
      int count = m_joystickPOVsCache[m_stick].m_count;
      if (m_sizedPOVs == null || m_sizedPOVs.length != count) {
        m_sizedPOVs = new long[count];
      }
      for (int i = 0; i < count; i++) {
        m_sizedPOVs[i] = m_joystickPOVsCache[m_stick].m_povs[i];
      }
      m_logPOVs.append(m_sizedPOVs, timestamp);
    }

    final int m_stick;
    boolean[] m_sizedButtons;
    float[] m_sizedAxes;
    long[] m_sizedPOVs;
    final BooleanArrayLogEntry m_logButtons;
    final FloatArrayLogEntry m_logAxes;
    final IntegerArrayLogEntry m_logPOVs;
  }

  private static class DataLogSender {
    DataLogSender(DataLog log, boolean logJoysticks, long timestamp) {
      m_logEnabled = new BooleanLogEntry(log, "DS:enabled", timestamp);
      m_logAutonomous = new BooleanLogEntry(log, "DS:autonomous", timestamp);
      m_logTest = new BooleanLogEntry(log, "DS:test", timestamp);
      m_logEstop = new BooleanLogEntry(log, "DS:estop", timestamp);

      // append initial control word values
      m_wasEnabled = m_controlWordCache.getEnabled();
      m_wasAutonomous = m_controlWordCache.getAutonomous();
      m_wasTest = m_controlWordCache.getTest();
      m_wasEstop = m_controlWordCache.getEStop();

      m_logEnabled.append(m_wasEnabled, timestamp);
      m_logAutonomous.append(m_wasAutonomous, timestamp);
      m_logTest.append(m_wasTest, timestamp);
      m_logEstop.append(m_wasEstop, timestamp);

      if (logJoysticks) {
        m_joysticks = new JoystickLogSender[kJoystickPorts];
        for (int i = 0; i < kJoystickPorts; i++) {
          m_joysticks[i] = new JoystickLogSender(log, i, timestamp);
        }
      } else {
        m_joysticks = new JoystickLogSender[0];
      }
    }

    public void send(long timestamp) {
      // append control word value changes
      boolean enabled = m_controlWordCache.getEnabled();
      if (enabled != m_wasEnabled) {
        m_logEnabled.append(enabled, timestamp);
      }
      m_wasEnabled = enabled;

      boolean autonomous = m_controlWordCache.getAutonomous();
      if (autonomous != m_wasAutonomous) {
        m_logAutonomous.append(autonomous, timestamp);
      }
      m_wasAutonomous = autonomous;

      boolean test = m_controlWordCache.getTest();
      if (test != m_wasTest) {
        m_logTest.append(test, timestamp);
      }
      m_wasTest = test;

      boolean estop = m_controlWordCache.getEStop();
      if (estop != m_wasEstop) {
        m_logEstop.append(estop, timestamp);
      }
      m_wasEstop = estop;

      // append joystick value changes
      for (JoystickLogSender joystick : m_joysticks) {
        joystick.send(timestamp);
      }
    }

    boolean m_wasEnabled;
    boolean m_wasAutonomous;
    boolean m_wasTest;
    boolean m_wasEstop;
    final BooleanLogEntry m_logEnabled;
    final BooleanLogEntry m_logAutonomous;
    final BooleanLogEntry m_logTest;
    final BooleanLogEntry m_logEstop;

    final JoystickLogSender[] m_joysticks;
  }

  private static DriverStation instance = new DriverStation();

  // Joystick User Data
  private static HALJoystickAxes[] m_joystickAxes = new HALJoystickAxes[kJoystickPorts];
  private static HALJoystickPOVs[] m_joystickPOVs = new HALJoystickPOVs[kJoystickPorts];
  private static HALJoystickButtons[] m_joystickButtons = new HALJoystickButtons[kJoystickPorts];
  private static MatchInfoData m_matchInfo = new MatchInfoData();

  // Joystick Cached Data
  private static HALJoystickAxes[] m_joystickAxesCache = new HALJoystickAxes[kJoystickPorts];
  private static HALJoystickPOVs[] m_joystickPOVsCache = new HALJoystickPOVs[kJoystickPorts];
  private static HALJoystickButtons[] m_joystickButtonsCache =
      new HALJoystickButtons[kJoystickPorts];
  private static MatchInfoData m_matchInfoCache = new MatchInfoData();

  // Joystick button rising/falling edge flags
  private static int[] m_joystickButtonsPressed = new int[kJoystickPorts];
  private static int[] m_joystickButtonsReleased = new int[kJoystickPorts];

  // preallocated byte buffer for button count
  private static final ByteBuffer m_buttonCountBuffer = ByteBuffer.allocateDirect(1);

  private static final MatchDataSender m_matchDataSender;
  private static DataLogSender m_dataLogSender;

  // Internal Driver Station thread
  private static Thread m_thread;

  private static volatile boolean m_threadKeepAlive = true;

  private static final ReentrantLock m_cacheDataMutex = new ReentrantLock();

  private static final Lock m_waitForDataMutex;
  private static final Condition m_waitForDataCond;
  private static int m_waitForDataCount;
  private static final ThreadLocal<Integer> m_lastCount = ThreadLocal.withInitial(() -> 0);

  private static boolean m_silenceJoystickWarning;

  // Robot state status variables
  private static boolean m_userInDisabled;
  private static boolean m_userInAutonomous;
  private static boolean m_userInTeleop;
  private static boolean m_userInTest;

  // Control word variables
  private static final ReentrantLock m_controlWordMutex = new ReentrantLock();
  private static final ControlWord m_controlWordCache;
  private static long m_lastControlWordUpdate;

  /**
   * Gets an instance of the DriverStation.
   *
   * @return The DriverStation.
   * @deprecated Use the static methods
   */
  @Deprecated
  public static DriverStation getInstance() {
    return DriverStation.instance;
  }

  /**
   * DriverStation constructor.
   *
   * <p>The single DriverStation instance is created statically with the instance static member
   * variable.
   */
  private DriverStation() {}

  static {
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

    m_controlWordCache = new ControlWord();
    m_lastControlWordUpdate = 0;

    m_matchDataSender = new MatchDataSender();

    m_thread = new Thread(new DriverStationTask(), "FRCDriverStation");
    m_thread.setPriority((Thread.NORM_PRIORITY + Thread.MAX_PRIORITY) / 2);

    m_thread.start();
  }

  /** Kill the thread. */
  public static synchronized void release() {
    m_threadKeepAlive = false;
    if (m_thread != null) {
      try {
        m_thread.join();
      } catch (InterruptedException ex) {
        Thread.currentThread().interrupt();
      }
      m_thread = null;
    }
  }

  /**
   * Report error to Driver Station. Optionally appends Stack trace to error message.
   *
   * @param error The error to report.
   * @param printTrace If true, append stack trace to error string
   */
  public static void reportError(String error, boolean printTrace) {
    reportErrorImpl(true, 1, error, printTrace);
  }

  /**
   * Report error to Driver Station. Appends provided stack trace to error message.
   *
   * @param error The error to report.
   * @param stackTrace The stack trace to append
   */
  public static void reportError(String error, StackTraceElement[] stackTrace) {
    reportErrorImpl(true, 1, error, stackTrace);
  }

  /**
   * Report warning to Driver Station. Optionally appends Stack trace to warning message.
   *
   * @param warning The warning to report.
   * @param printTrace If true, append stack trace to warning string
   */
  public static void reportWarning(String warning, boolean printTrace) {
    reportErrorImpl(false, 1, warning, printTrace);
  }

  /**
   * Report warning to Driver Station. Appends provided stack trace to warning message.
   *
   * @param warning The warning to report.
   * @param stackTrace The stack trace to append
   */
  public static void reportWarning(String warning, StackTraceElement[] stackTrace) {
    reportErrorImpl(false, 1, warning, stackTrace);
  }

  private static void reportErrorImpl(boolean isError, int code, String error, boolean printTrace) {
    reportErrorImpl(isError, code, error, printTrace, Thread.currentThread().getStackTrace(), 3);
  }

  private static void reportErrorImpl(
      boolean isError, int code, String error, StackTraceElement[] stackTrace) {
    reportErrorImpl(isError, code, error, true, stackTrace, 0);
  }

  private static void reportErrorImpl(
      boolean isError,
      int code,
      String error,
      boolean printTrace,
      StackTraceElement[] stackTrace,
      int stackTraceFirst) {
    String locString;
    if (stackTrace.length >= stackTraceFirst + 1) {
      locString = stackTrace[stackTraceFirst].toString();
    } else {
      locString = "";
    }
    StringBuilder traceString = new StringBuilder();
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
   * @param stick The joystick to read.
   * @param button The button index, beginning at 1.
   * @return The state of the joystick button.
   */
  public static boolean getStickButton(final int stick, final int button) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }
    if (button <= 0) {
      reportJoystickUnpluggedError("Button indexes begin at 1 in WPILib for C++ and Java\n");
      return false;
    }

    m_cacheDataMutex.lock();
    try {
      if (button <= m_joystickButtons[stick].m_count) {
        return (m_joystickButtons[stick].m_buttons & 1 << (button - 1)) != 0;
      }
    } finally {
      m_cacheDataMutex.unlock();
    }

    reportJoystickUnpluggedWarning(
        "Joystick Button "
            + button
            + " on port "
            + stick
            + " not available, check if controller is plugged in");
    return false;
  }

  /**
   * Whether one joystick button was pressed since the last check. Button indexes begin at 1.
   *
   * @param stick The joystick to read.
   * @param button The button index, beginning at 1.
   * @return Whether the joystick button was pressed since the last check.
   */
  public static boolean getStickButtonPressed(final int stick, final int button) {
    if (button <= 0) {
      reportJoystickUnpluggedError("Button indexes begin at 1 in WPILib for C++ and Java\n");
      return false;
    }
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    m_cacheDataMutex.lock();
    try {
      if (button <= m_joystickButtons[stick].m_count) {
        // If button was pressed, clear flag and return true
        if ((m_joystickButtonsPressed[stick] & 1 << (button - 1)) != 0) {
          m_joystickButtonsPressed[stick] &= ~(1 << (button - 1));
          return true;
        } else {
          return false;
        }
      }
    } finally {
      m_cacheDataMutex.unlock();
    }

    reportJoystickUnpluggedWarning(
        "Joystick Button "
            + button
            + " on port "
            + stick
            + " not available, check if controller is plugged in");
    return false;
  }

  /**
   * Whether one joystick button was released since the last check. Button indexes begin at 1.
   *
   * @param stick The joystick to read.
   * @param button The button index, beginning at 1.
   * @return Whether the joystick button was released since the last check.
   */
  public static boolean getStickButtonReleased(final int stick, final int button) {
    if (button <= 0) {
      reportJoystickUnpluggedError("Button indexes begin at 1 in WPILib for C++ and Java\n");
      return false;
    }
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    m_cacheDataMutex.lock();
    try {
      if (button <= m_joystickButtons[stick].m_count) {
        // If button was released, clear flag and return true
        if ((m_joystickButtonsReleased[stick] & 1 << (button - 1)) != 0) {
          m_joystickButtonsReleased[stick] &= ~(1 << (button - 1));
          return true;
        } else {
          return false;
        }
      }
    } finally {
      m_cacheDataMutex.unlock();
    }

    reportJoystickUnpluggedWarning(
        "Joystick Button "
            + button
            + " on port "
            + stick
            + " not available, check if controller is plugged in");
    return false;
  }

  /**
   * Get the value of the axis on a joystick. This depends on the mapping of the joystick connected
   * to the specified port.
   *
   * @param stick The joystick to read.
   * @param axis The analog axis value to read from the joystick.
   * @return The value of the axis on the joystick.
   */
  public static double getStickAxis(int stick, int axis) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }
    if (axis < 0 || axis >= HAL.kMaxJoystickAxes) {
      throw new IllegalArgumentException("Joystick axis is out of range");
    }

    m_cacheDataMutex.lock();
    try {
      if (axis < m_joystickAxes[stick].m_count) {
        return m_joystickAxes[stick].m_axes[axis];
      }
    } finally {
      m_cacheDataMutex.unlock();
    }

    reportJoystickUnpluggedWarning(
        "Joystick axis "
            + axis
            + " on port "
            + stick
            + " not available, check if controller is plugged in");
    return 0.0;
  }

  /**
   * Get the state of a POV on the joystick.
   *
   * @param stick The joystick to read.
   * @param pov The POV to read.
   * @return the angle of the POV in degrees, or -1 if the POV is not pressed.
   */
  public static int getStickPOV(int stick, int pov) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }
    if (pov < 0 || pov >= HAL.kMaxJoystickPOVs) {
      throw new IllegalArgumentException("Joystick POV is out of range");
    }

    m_cacheDataMutex.lock();
    try {
      if (pov < m_joystickPOVs[stick].m_count) {
        return m_joystickPOVs[stick].m_povs[pov];
      }
    } finally {
      m_cacheDataMutex.unlock();
    }

    reportJoystickUnpluggedWarning(
        "Joystick POV "
            + pov
            + " on port "
            + stick
            + " not available, check if controller is plugged in");
    return -1;
  }

  /**
   * The state of the buttons on the joystick.
   *
   * @param stick The joystick to read.
   * @return The state of the buttons on the joystick.
   */
  public static int getStickButtons(final int stick) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
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
  public static int getStickAxisCount(int stick) {
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
  public static int getStickPOVCount(int stick) {
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
  public static int getStickButtonCount(int stick) {
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
  public static boolean getJoystickIsXbox(int stick) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    return HAL.getJoystickIsXbox((byte) stick) == 1;
  }

  /**
   * Gets the value of type on a joystick.
   *
   * @param stick The joystick port number
   * @return The value of type
   */
  public static int getJoystickType(int stick) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    return HAL.getJoystickType((byte) stick);
  }

  /**
   * Gets the name of the joystick at a port.
   *
   * @param stick The joystick port number
   * @return The value of name
   */
  public static String getJoystickName(int stick) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    return HAL.getJoystickName((byte) stick);
  }

  /**
   * Returns the types of Axes on a given joystick port.
   *
   * @param stick The joystick port number
   * @param axis The target axis
   * @return What type of axis the axis is reporting to be
   */
  public static int getJoystickAxisType(int stick, int axis) {
    if (stick < 0 || stick >= kJoystickPorts) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    return HAL.getJoystickAxisType((byte) stick, (byte) axis);
  }

  /**
   * Returns if a joystick is connected to the Driver Station.
   *
   * <p>This makes a best effort guess by looking at the reported number of axis, buttons, and POVs
   * attached.
   *
   * @param stick The joystick port number
   * @return true if a joystick is connected
   */
  public static boolean isJoystickConnected(int stick) {
    return getStickAxisCount(stick) > 0
        || getStickButtonCount(stick) > 0
        || getStickPOVCount(stick) > 0;
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be enabled.
   *
   * @return True if the robot is enabled, false otherwise.
   */
  public static boolean isEnabled() {
    m_controlWordMutex.lock();
    try {
      updateControlWord(false);
      return m_controlWordCache.getEnabled() && m_controlWordCache.getDSAttached();
    } finally {
      m_controlWordMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be disabled.
   *
   * @return True if the robot should be disabled, false otherwise.
   */
  public static boolean isDisabled() {
    return !isEnabled();
  }

  /**
   * Gets a value indicating whether the Robot is e-stopped.
   *
   * @return True if the robot is e-stopped, false otherwise.
   */
  public static boolean isEStopped() {
    m_controlWordMutex.lock();
    try {
      updateControlWord(false);
      return m_controlWordCache.getEStop();
    } finally {
      m_controlWordMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode.
   *
   * @return True if autonomous mode should be enabled, false otherwise.
   */
  public static boolean isAutonomous() {
    m_controlWordMutex.lock();
    try {
      updateControlWord(false);
      return m_controlWordCache.getAutonomous();
    } finally {
      m_controlWordMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode and enabled.
   *
   * @return True if autonomous should be set and the robot should be enabled.
   */
  public static boolean isAutonomousEnabled() {
    m_controlWordMutex.lock();
    try {
      updateControlWord(false);
      return m_controlWordCache.getAutonomous() && m_controlWordCache.getEnabled();
    } finally {
      m_controlWordMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controlled mode.
   *
   * @return True if operator-controlled mode should be enabled, false otherwise.
   * @deprecated Use isTeleop() instead.
   */
  @Deprecated(since = "2022", forRemoval = true)
  public static boolean isOperatorControl() {
    return isTeleop();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controlled mode.
   *
   * @return True if operator-controlled mode should be enabled, false otherwise.
   */
  public static boolean isTeleop() {
    return !(isAutonomous() || isTest());
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controller mode and enabled.
   *
   * @return True if operator-controlled mode should be set and the robot should be enabled.
   * @deprecated Use isTeleopEnabled() instead.
   */
  @Deprecated(since = "2022", forRemoval = true)
  public static boolean isOperatorControlEnabled() {
    return isTeleopEnabled();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controller mode and enabled.
   *
   * @return True if operator-controlled mode should be set and the robot should be enabled.
   */
  public static boolean isTeleopEnabled() {
    m_controlWordMutex.lock();
    try {
      updateControlWord(false);
      return !m_controlWordCache.getAutonomous()
          && !m_controlWordCache.getTest()
          && m_controlWordCache.getEnabled();
    } finally {
      m_controlWordMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in test
   * mode.
   *
   * @return True if test mode should be enabled, false otherwise.
   */
  public static boolean isTest() {
    m_controlWordMutex.lock();
    try {
      updateControlWord(false);
      return m_controlWordCache.getTest();
    } finally {
      m_controlWordMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station is attached.
   *
   * @return True if Driver Station is attached, false otherwise.
   */
  public static boolean isDSAttached() {
    m_controlWordMutex.lock();
    try {
      updateControlWord(false);
      return m_controlWordCache.getDSAttached();
    } finally {
      m_controlWordMutex.unlock();
    }
  }

  /**
   * Gets if a new control packet from the driver station arrived since the last time this function
   * was called.
   *
   * @return True if the control data has been updated since the last call.
   */
  public static boolean isNewControlData() {
    m_waitForDataMutex.lock();
    try {
      int currentCount = m_waitForDataCount;
      if (m_lastCount.get() != currentCount) {
        m_lastCount.set(currentCount);
        return true;
      }
    } finally {
      m_waitForDataMutex.unlock();
    }
    return false;
  }

  /**
   * Gets if the driver station attached to a Field Management System.
   *
   * @return true if the robot is competing on a field being controlled by a Field Management System
   */
  public static boolean isFMSAttached() {
    m_controlWordMutex.lock();
    try {
      updateControlWord(false);
      return m_controlWordCache.getFMSAttached();
    } finally {
      m_controlWordMutex.unlock();
    }
  }

  /**
   * Get the game specific message.
   *
   * @return the game specific message
   */
  public static String getGameSpecificMessage() {
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
  public static String getEventName() {
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
  public static MatchType getMatchType() {
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
  public static int getMatchNumber() {
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
  public static int getReplayNumber() {
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
  public static Alliance getAlliance() {
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
  public static int getLocation() {
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
   *
   * <p>Checks if new control data has arrived since the last waitForData call on the current
   * thread. If new data has not arrived, returns immediately.
   */
  public static void waitForData() {
    waitForData(0);
  }

  /**
   * Wait for new data or for timeout, which ever comes first. If timeout is 0, wait for new data
   * only.
   *
   * <p>Checks if new control data has arrived since the last waitForData call on the current
   * thread. If new data has not arrived, returns immediately.
   *
   * @param timeoutSeconds The maximum time in seconds to wait.
   * @return true if there is new data, otherwise false
   */
  public static boolean waitForData(double timeoutSeconds) {
    long startTimeMicroS = RobotController.getFPGATime();
    long timeoutMicroS = (long) (timeoutSeconds * 1e6);
    m_waitForDataMutex.lock();
    try {
      int currentCount = m_waitForDataCount;
      if (m_lastCount.get() != currentCount) {
        m_lastCount.set(currentCount);
        return true;
      }
      while (m_waitForDataCount == currentCount) {
        if (timeoutMicroS > 0) {
          long nowMicroS = RobotController.getFPGATime();
          if (nowMicroS < startTimeMicroS + timeoutMicroS) {
            // We still have time to wait
            boolean signaled =
                m_waitForDataCond.await(
                    startTimeMicroS + timeoutMicroS - nowMicroS, TimeUnit.MICROSECONDS);
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
      m_lastCount.set(m_waitForDataCount);
      // Return true if we have received a proper signal
      return true;
    } catch (InterruptedException ex) {
      // return false on a thread interrupt
      Thread.currentThread().interrupt();
      return false;
    } finally {
      m_waitForDataMutex.unlock();
    }
  }

  /**
   * Return the approximate match time. The FMS does not send an official match time to the robots,
   * but does send an approximate match time. The value will count down the time remaining in the
   * current period (auto or teleop). Warning: This is not an official time (so it cannot be used to
   * dispute ref calls or guarantee that a function will trigger before the match ends) The Practice
   * Match function of the DS approximates the behavior seen on the field.
   *
   * @return Time remaining in current match period (auto or teleop) in seconds
   */
  public static double getMatchTime() {
    return HAL.getMatchTime();
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting disabled code; if false, leaving disabled code
   */
  public static void inDisabled(boolean entering) {
    m_userInDisabled = entering;
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting autonomous code; if false, leaving autonomous code
   */
  public static void inAutonomous(boolean entering) {
    m_userInAutonomous = entering;
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting teleop code; if false, leaving teleop code
   * @deprecated Use {@link #inTeleop(boolean)} instead.
   */
  @Deprecated(since = "2022", forRemoval = true)
  public static void inOperatorControl(boolean entering) {
    m_userInTeleop = entering;
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting teleop code; if false, leaving teleop code
   */
  public static void inTeleop(boolean entering) {
    m_userInTeleop = entering;
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting test code; if false, leaving test code
   */
  public static void inTest(boolean entering) {
    m_userInTest = entering;
  }

  /** Forces waitForData() to return immediately. */
  public static void wakeupWaitForData() {
    m_waitForDataMutex.lock();
    try {
      m_waitForDataCount++;
      m_waitForDataCond.signalAll();
    } finally {
      m_waitForDataMutex.unlock();
    }
  }

  /**
   * Allows the user to specify whether they want joystick connection warnings to be printed to the
   * console. This setting is ignored when the FMS is connected -- warnings will always be on in
   * that scenario.
   *
   * @param silence Whether warning messages should be silenced.
   */
  public static void silenceJoystickConnectionWarning(boolean silence) {
    m_silenceJoystickWarning = silence;
  }

  /**
   * Returns whether joystick connection warnings are silenced. This will always return false when
   * connected to the FMS.
   *
   * @return Whether joystick connection warnings are silenced.
   */
  public static boolean isJoystickConnectionWarningSilenced() {
    return !isFMSAttached() && m_silenceJoystickWarning;
  }

  /**
   * Copy data from the DS task for the user. If no new data exists, it will just be returned,
   * otherwise the data will be copied from the DS polling loop.
   */
  protected static void getData() {
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

    m_controlWordMutex.lock();
    try {
      // Force a control word update, to make sure the data is the newest.
      updateControlWord(true);
    } finally {
      m_controlWordMutex.unlock();
    }

    DataLogSender dataLogSender;
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

      dataLogSender = m_dataLogSender;
    } finally {
      m_cacheDataMutex.unlock();
    }

    wakeupWaitForData();
    m_matchDataSender.sendMatchData();
    if (dataLogSender != null) {
      dataLogSender.send(WPIUtilJNI.now());
    }
  }

  /**
   * Reports errors related to unplugged joysticks Throttles the errors so that they don't overwhelm
   * the DS.
   */
  private static void reportJoystickUnpluggedError(String message) {
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
  private static void reportJoystickUnpluggedWarning(String message) {
    if (isFMSAttached() || !m_silenceJoystickWarning) {
      double currentTime = Timer.getFPGATimestamp();
      if (currentTime > m_nextMessageTime) {
        reportWarning(message, false);
        m_nextMessageTime = currentTime + JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL;
      }
    }
  }

  /** Provides the service routine for the DS polling m_thread. */
  private static void run() {
    int safetyCounter = 0;
    while (m_threadKeepAlive) {
      HAL.waitForDSData();
      getData();

      if (isDisabled()) {
        safetyCounter = 0;
      }

      safetyCounter++;
      if (safetyCounter >= 4) {
        MotorSafety.checkMotors();
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
   * Forces a control word cache update, and update the passed in control word.
   *
   * @param word Word to update.
   */
  public static void updateControlWordFromCache(ControlWord word) {
    m_controlWordMutex.lock();
    try {
      updateControlWord(true);
      word.update(m_controlWordCache);
    } finally {
      m_controlWordMutex.unlock();
    }
  }

  /**
   * Updates the data in the control word cache. Updates if the force parameter is set, or if 50ms
   * have passed since the last update.
   *
   * <p>Must be called with m_controlWordMutex lock held.
   *
   * @param force True to force an update to the cache, otherwise update if 50ms have passed.
   */
  private static void updateControlWord(boolean force) {
    long now = System.currentTimeMillis();
    if (now - m_lastControlWordUpdate > 50 || force) {
      HAL.getControlWord(m_controlWordCache);
      m_lastControlWordUpdate = now;
    }
  }

  /**
   * Starts logging DriverStation data to data log. Repeated calls are ignored.
   *
   * @param log data log
   * @param logJoysticks if true, log joystick data
   */
  @SuppressWarnings("PMD.NonThreadSafeSingleton")
  public static void startDataLog(DataLog log, boolean logJoysticks) {
    m_cacheDataMutex.lock();
    try {
      if (m_dataLogSender == null) {
        m_dataLogSender = new DataLogSender(log, logJoysticks, WPIUtilJNI.now());
      }
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Starts logging DriverStation data to data log, including joystick data. Repeated calls are
   * ignored.
   *
   * @param log data log
   */
  public static void startDataLog(DataLog log) {
    startDataLog(log, true);
  }
}
