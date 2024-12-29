// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.AllianceStationID;
import edu.wpi.first.hal.ControlWord;
import edu.wpi.first.hal.DriverStationJNI;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.MatchInfoData;
import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.IntegerPublisher;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.StringPublisher;
import edu.wpi.first.networktables.StringTopic;
import edu.wpi.first.util.EventVector;
import edu.wpi.first.util.WPIUtilJNI;
import edu.wpi.first.util.datalog.BooleanArrayLogEntry;
import edu.wpi.first.util.datalog.BooleanLogEntry;
import edu.wpi.first.util.datalog.DataLog;
import edu.wpi.first.util.datalog.FloatArrayLogEntry;
import edu.wpi.first.util.datalog.IntegerArrayLogEntry;
import java.nio.ByteBuffer;
import java.util.Map;
import java.util.Optional;
import java.util.OptionalInt;
import java.util.concurrent.locks.ReentrantLock;

/** Provide access to the network communication data to / from the Driver Station. */
public final class DriverStation {
  /** Number of Joystick ports. */
  public static final int kJoystickPorts = 6;

  private static final class HALJoystickButtons {
    public int m_buttons;
    public byte m_count;
  }

  private static class HALJoystickAxes {
    public final float[] m_axes;
    public int m_count;

    HALJoystickAxes(int count) {
      m_axes = new float[count];
    }
  }

  private static class HALJoystickAxesRaw {
    public final int[] m_axes;

    @SuppressWarnings("unused")
    public int m_count;

    HALJoystickAxesRaw(int count) {
      m_axes = new int[count];
    }
  }

  private static class HALJoystickPOVs {
    public final short[] m_povs;
    public int m_count;

    HALJoystickPOVs(int count) {
      m_povs = new short[count];
      for (int i = 0; i < count; i++) {
        m_povs[i] = -1;
      }
    }
  }

  /** The robot alliance that the robot is a part of. */
  public enum Alliance {
    /** Red alliance. */
    Red,
    /** Blue alliance. */
    Blue
  }

  /** The type of robot match that the robot is part of. */
  public enum MatchType {
    /** None. */
    None,
    /** Practice. */
    Practice,
    /** Qualification. */
    Qualification,
    /** Elimination. */
    Elimination
  }

  private static final double JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL = 1.0;
  private static double m_nextMessageTime;

  @SuppressWarnings("MemberName")
  private static class MatchDataSender {
    private static final String kSmartDashboardType = "FMSInfo";

    final StringPublisher gameSpecificMessage;
    final StringPublisher eventName;
    final IntegerPublisher matchNumber;
    final IntegerPublisher replayNumber;
    final IntegerPublisher matchType;
    final BooleanPublisher alliance;
    final IntegerPublisher station;
    final IntegerPublisher controlWord;
    boolean oldIsRedAlliance = true;
    int oldStationNumber = 1;
    String oldEventName = "";
    String oldGameSpecificMessage = "";
    int oldMatchNumber;
    int oldReplayNumber;
    int oldMatchType;
    int oldControlWord;

    MatchDataSender() {
      var table = NetworkTableInstance.getDefault().getTable("FMSInfo");
      table
          .getStringTopic(".type")
          .publishEx(
              StringTopic.kTypeString, "{\"SmartDashboard\":\"" + kSmartDashboardType + "\"}")
          .set(kSmartDashboardType);
      gameSpecificMessage = table.getStringTopic("GameSpecificMessage").publish();
      gameSpecificMessage.set("");
      eventName = table.getStringTopic("EventName").publish();
      eventName.set("");
      matchNumber = table.getIntegerTopic("MatchNumber").publish();
      matchNumber.set(0);
      replayNumber = table.getIntegerTopic("ReplayNumber").publish();
      replayNumber.set(0);
      matchType = table.getIntegerTopic("MatchType").publish();
      matchType.set(0);
      alliance = table.getBooleanTopic("IsRedAlliance").publish();
      alliance.set(true);
      station = table.getIntegerTopic("StationNumber").publish();
      station.set(1);
      controlWord = table.getIntegerTopic("FMSControlData").publish();
      controlWord.set(0);
    }

    private void sendMatchData() {
      AllianceStationID allianceID = DriverStationJNI.getAllianceStation();
      final int stationNumber =
          switch (allianceID) {
            case Blue1, Red1 -> 1;
            case Blue2, Red2 -> 2;
            case Blue3, Red3, Unknown -> 3;
          };
      final boolean isRedAlliance =
          switch (allianceID) {
            case Blue1, Blue2, Blue3 -> false;
            case Red1, Red2, Red3, Unknown -> true;
          };

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
      currentControlWord = DriverStationJNI.nativeGetControlWord();

      if (oldIsRedAlliance != isRedAlliance) {
        alliance.set(isRedAlliance);
        oldIsRedAlliance = isRedAlliance;
      }
      if (oldStationNumber != stationNumber) {
        station.set(stationNumber);
        oldStationNumber = stationNumber;
      }
      if (!oldEventName.equals(currentEventName)) {
        eventName.set(currentEventName);
        oldEventName = currentEventName;
      }
      if (!oldGameSpecificMessage.equals(currentGameSpecificMessage)) {
        gameSpecificMessage.set(currentGameSpecificMessage);
        oldGameSpecificMessage = currentGameSpecificMessage;
      }
      if (currentMatchNumber != oldMatchNumber) {
        matchNumber.set(currentMatchNumber);
        oldMatchNumber = currentMatchNumber;
      }
      if (currentReplayNumber != oldReplayNumber) {
        replayNumber.set(currentReplayNumber);
        oldReplayNumber = currentReplayNumber;
      }
      if (currentMatchType != oldMatchType) {
        matchType.set(currentMatchType);
        oldMatchType = currentMatchType;
      }
      if (currentControlWord != oldControlWord) {
        controlWord.set(currentControlWord);
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

      appendButtons(m_joystickButtons[m_stick], timestamp);
      appendAxes(m_joystickAxes[m_stick], timestamp);
      appendPOVs(m_joystickPOVs[m_stick], timestamp);
    }

    public void send(long timestamp) {
      HALJoystickButtons buttons = m_joystickButtons[m_stick];
      if (buttons.m_count != m_prevButtons.m_count
          || buttons.m_buttons != m_prevButtons.m_buttons) {
        appendButtons(buttons, timestamp);
      }

      HALJoystickAxes axes = m_joystickAxes[m_stick];
      int count = axes.m_count;
      boolean needToLog = false;
      if (count != m_prevAxes.m_count) {
        needToLog = true;
      } else {
        for (int i = 0; i < count; i++) {
          if (axes.m_axes[i] != m_prevAxes.m_axes[i]) {
            needToLog = true;
            break;
          }
        }
      }
      if (needToLog) {
        appendAxes(axes, timestamp);
      }

      HALJoystickPOVs povs = m_joystickPOVs[m_stick];
      count = m_joystickPOVs[m_stick].m_count;
      needToLog = false;
      if (count != m_prevPOVs.m_count) {
        needToLog = true;
      } else {
        for (int i = 0; i < count; i++) {
          if (povs.m_povs[i] != m_prevPOVs.m_povs[i]) {
            needToLog = true;
            break;
          }
        }
      }
      if (needToLog) {
        appendPOVs(povs, timestamp);
      }
    }

    void appendButtons(HALJoystickButtons buttons, long timestamp) {
      byte count = buttons.m_count;
      if (m_sizedButtons == null || m_sizedButtons.length != count) {
        m_sizedButtons = new boolean[count];
      }
      int buttonsValue = buttons.m_buttons;
      for (int i = 0; i < count; i++) {
        m_sizedButtons[i] = (buttonsValue & (1 << i)) != 0;
      }
      m_logButtons.append(m_sizedButtons, timestamp);
      m_prevButtons.m_count = count;
      m_prevButtons.m_buttons = buttons.m_buttons;
    }

    void appendAxes(HALJoystickAxes axes, long timestamp) {
      int count = axes.m_count;
      if (m_sizedAxes == null || m_sizedAxes.length != count) {
        m_sizedAxes = new float[count];
      }
      System.arraycopy(axes.m_axes, 0, m_sizedAxes, 0, count);
      m_logAxes.append(m_sizedAxes, timestamp);
      m_prevAxes.m_count = count;
      System.arraycopy(axes.m_axes, 0, m_prevAxes.m_axes, 0, count);
    }

    @SuppressWarnings("PMD.AvoidArrayLoops")
    void appendPOVs(HALJoystickPOVs povs, long timestamp) {
      int count = povs.m_count;
      if (m_sizedPOVs == null || m_sizedPOVs.length != count) {
        m_sizedPOVs = new long[count];
      }
      for (int i = 0; i < count; i++) {
        m_sizedPOVs[i] = povs.m_povs[i];
      }
      m_logPOVs.append(m_sizedPOVs, timestamp);
      m_prevPOVs.m_count = count;
      System.arraycopy(povs.m_povs, 0, m_prevPOVs.m_povs, 0, count);
    }

    final int m_stick;
    boolean[] m_sizedButtons;
    float[] m_sizedAxes;
    long[] m_sizedPOVs;
    final HALJoystickButtons m_prevButtons = new HALJoystickButtons();
    final HALJoystickAxes m_prevAxes = new HALJoystickAxes(DriverStationJNI.kMaxJoystickAxes);
    final HALJoystickPOVs m_prevPOVs = new HALJoystickPOVs(DriverStationJNI.kMaxJoystickPOVs);
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

  // Joystick User Data
  private static HALJoystickAxes[] m_joystickAxes = new HALJoystickAxes[kJoystickPorts];
  private static HALJoystickAxesRaw[] m_joystickAxesRaw = new HALJoystickAxesRaw[kJoystickPorts];
  private static HALJoystickPOVs[] m_joystickPOVs = new HALJoystickPOVs[kJoystickPorts];
  private static HALJoystickButtons[] m_joystickButtons = new HALJoystickButtons[kJoystickPorts];
  private static MatchInfoData m_matchInfo = new MatchInfoData();
  private static ControlWord m_controlWord = new ControlWord();
  private static EventVector m_refreshEvents = new EventVector();

  // Joystick Cached Data
  private static HALJoystickAxes[] m_joystickAxesCache = new HALJoystickAxes[kJoystickPorts];
  private static HALJoystickAxesRaw[] m_joystickAxesRawCache =
      new HALJoystickAxesRaw[kJoystickPorts];
  private static HALJoystickPOVs[] m_joystickPOVsCache = new HALJoystickPOVs[kJoystickPorts];
  private static HALJoystickButtons[] m_joystickButtonsCache =
      new HALJoystickButtons[kJoystickPorts];
  private static MatchInfoData m_matchInfoCache = new MatchInfoData();
  private static ControlWord m_controlWordCache = new ControlWord();

  // Joystick button rising/falling edge flags
  private static int[] m_joystickButtonsPressed = new int[kJoystickPorts];
  private static int[] m_joystickButtonsReleased = new int[kJoystickPorts];

  // preallocated byte buffer for button count
  private static final ByteBuffer m_buttonCountBuffer = ByteBuffer.allocateDirect(1);

  private static final MatchDataSender m_matchDataSender;
  private static DataLogSender m_dataLogSender;

  private static final ReentrantLock m_cacheDataMutex = new ReentrantLock();

  private static boolean m_silenceJoystickWarning;

  /**
   * DriverStation constructor.
   *
   * <p>The single DriverStation instance is created statically with the instance static member
   * variable.
   */
  private DriverStation() {}

  static {
    HAL.initialize(500, 0);

    for (int i = 0; i < kJoystickPorts; i++) {
      m_joystickButtons[i] = new HALJoystickButtons();
      m_joystickAxes[i] = new HALJoystickAxes(DriverStationJNI.kMaxJoystickAxes);
      m_joystickAxesRaw[i] = new HALJoystickAxesRaw(DriverStationJNI.kMaxJoystickAxes);
      m_joystickPOVs[i] = new HALJoystickPOVs(DriverStationJNI.kMaxJoystickPOVs);

      m_joystickButtonsCache[i] = new HALJoystickButtons();
      m_joystickAxesCache[i] = new HALJoystickAxes(DriverStationJNI.kMaxJoystickAxes);
      m_joystickAxesRawCache[i] = new HALJoystickAxesRaw(DriverStationJNI.kMaxJoystickAxes);
      m_joystickPOVsCache[i] = new HALJoystickPOVs(DriverStationJNI.kMaxJoystickPOVs);
    }

    m_matchDataSender = new MatchDataSender();
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
    DriverStationJNI.sendError(
        isError, code, false, error, locString, traceString.toString(), true);
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
    if (axis < 0 || axis >= DriverStationJNI.kMaxJoystickAxes) {
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
    if (pov < 0 || pov >= DriverStationJNI.kMaxJoystickPOVs) {
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
   * Returns the number of povs on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The number of povs on the indicated joystick
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

    return DriverStationJNI.getJoystickIsXbox((byte) stick) == 1;
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

    return DriverStationJNI.getJoystickType((byte) stick);
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

    return DriverStationJNI.getJoystickName((byte) stick);
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

    return DriverStationJNI.getJoystickAxisType((byte) stick, (byte) axis);
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
    m_cacheDataMutex.lock();
    try {
      return m_controlWord.getEnabled() && m_controlWord.getDSAttached();
    } finally {
      m_cacheDataMutex.unlock();
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
    m_cacheDataMutex.lock();
    try {
      return m_controlWord.getEStop();
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode.
   *
   * @return True if autonomous mode should be enabled, false otherwise.
   */
  public static boolean isAutonomous() {
    m_cacheDataMutex.lock();
    try {
      return m_controlWord.getAutonomous();
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode and enabled.
   *
   * @return True if autonomous should be set and the robot should be enabled.
   */
  public static boolean isAutonomousEnabled() {
    m_cacheDataMutex.lock();
    try {
      return m_controlWord.getAutonomous() && m_controlWord.getEnabled();
    } finally {
      m_cacheDataMutex.unlock();
    }
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
   */
  public static boolean isTeleopEnabled() {
    m_cacheDataMutex.lock();
    try {
      return !m_controlWord.getAutonomous()
          && !m_controlWord.getTest()
          && m_controlWord.getEnabled();
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in Test
   * mode.
   *
   * @return True if test mode should be enabled, false otherwise.
   */
  public static boolean isTest() {
    m_cacheDataMutex.lock();
    try {
      return m_controlWord.getTest();
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in Test
   * mode and enabled.
   *
   * @return True if test mode should be set and the robot should be enabled.
   */
  public static boolean isTestEnabled() {
    m_cacheDataMutex.lock();
    try {
      return m_controlWord.getTest() && m_controlWord.getEnabled();
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station is attached.
   *
   * @return True if Driver Station is attached, false otherwise.
   */
  public static boolean isDSAttached() {
    m_cacheDataMutex.lock();
    try {
      return m_controlWord.getDSAttached();
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets if the driver station attached to a Field Management System.
   *
   * @return true if the robot is competing on a field being controlled by a Field Management System
   */
  public static boolean isFMSAttached() {
    m_cacheDataMutex.lock();
    try {
      return m_controlWord.getFMSAttached();
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Get the game specific message from the FMS.
   *
   * <p>If the FMS is not connected, it is set from the game data setting on the driver station.
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
   * Get the event name from the FMS.
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
   * Get the match type from the FMS.
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
    return switch (matchType) {
      case 1 -> MatchType.Practice;
      case 2 -> MatchType.Qualification;
      case 3 -> MatchType.Elimination;
      default -> MatchType.None;
    };
  }

  /**
   * Get the match number from the FMS.
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
   * Get the replay number from the FMS.
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

  private static Map<AllianceStationID, Optional<Alliance>> m_allianceMap =
      Map.of(
          AllianceStationID.Unknown, Optional.empty(),
          AllianceStationID.Red1, Optional.of(Alliance.Red),
          AllianceStationID.Red2, Optional.of(Alliance.Red),
          AllianceStationID.Red3, Optional.of(Alliance.Red),
          AllianceStationID.Blue1, Optional.of(Alliance.Blue),
          AllianceStationID.Blue2, Optional.of(Alliance.Blue),
          AllianceStationID.Blue3, Optional.of(Alliance.Blue));

  private static Map<AllianceStationID, OptionalInt> m_stationMap =
      Map.of(
          AllianceStationID.Unknown, OptionalInt.empty(),
          AllianceStationID.Red1, OptionalInt.of(1),
          AllianceStationID.Red2, OptionalInt.of(2),
          AllianceStationID.Red3, OptionalInt.of(3),
          AllianceStationID.Blue1, OptionalInt.of(1),
          AllianceStationID.Blue2, OptionalInt.of(2),
          AllianceStationID.Blue3, OptionalInt.of(3));

  /**
   * Get the current alliance from the FMS.
   *
   * <p>If the FMS is not connected, it is set from the team alliance setting on the driver station.
   *
   * @return The alliance (red or blue) or an empty optional if the alliance is invalid
   */
  public static Optional<Alliance> getAlliance() {
    AllianceStationID allianceStationID = DriverStationJNI.getAllianceStation();
    if (allianceStationID == null) {
      allianceStationID = AllianceStationID.Unknown;
    }

    return m_allianceMap.get(allianceStationID);
  }

  /**
   * Gets the location of the team's driver station controls from the FMS.
   *
   * <p>If the FMS is not connected, it is set from the team alliance setting on the driver station.
   *
   * @return the location of the team's driver station controls: 1, 2, or 3
   */
  public static OptionalInt getLocation() {
    AllianceStationID allianceStationID = DriverStationJNI.getAllianceStation();
    if (allianceStationID == null) {
      allianceStationID = AllianceStationID.Unknown;
    }

    return m_stationMap.get(allianceStationID);
  }

  /**
   * Gets the raw alliance station of the teams driver station.
   *
   * <p>This returns the raw low level value. Prefer getLocation or getAlliance unless necessary for
   * performance.
   *
   * @return The raw alliance station id.
   */
  public static AllianceStationID getRawAllianceStation() {
    return DriverStationJNI.getAllianceStation();
  }

  /**
   * Wait for a DS connection.
   *
   * @param timeoutSeconds timeout in seconds. 0 for infinite.
   * @return true if connected, false if timeout
   */
  public static boolean waitForDsConnection(double timeoutSeconds) {
    int event = WPIUtilJNI.createEvent(true, false);
    DriverStationJNI.provideNewDataEventHandle(event);
    boolean result;
    try {
      if (timeoutSeconds == 0) {
        WPIUtilJNI.waitForObject(event);
        result = true;
      } else {
        result = !WPIUtilJNI.waitForObjectTimeout(event, timeoutSeconds);
      }
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      result = false;
    } finally {
      DriverStationJNI.removeNewDataEventHandle(event);
      WPIUtilJNI.destroyEvent(event);
    }
    return result;
  }

  /**
   * Return the approximate match time. The FMS does not send an official match time to the robots,
   * but does send an approximate match time. The value will count down the time remaining in the
   * current period (auto or teleop). Warning: This is not an official time (so it cannot be used to
   * dispute ref calls or guarantee that a function will trigger before the match ends).
   *
   * <p>When connected to the real field, this number only changes in full integer increments, and
   * always counts down.
   *
   * <p>When the DS is in practice mode, this number is a floating point number, and counts down.
   *
   * <p>When the DS is in teleop or autonomous mode, this number is a floating point number, and
   * counts up.
   *
   * <p>Simulation matches DS behavior without an FMS connected.
   *
   * @return Time remaining in current match period (auto or teleop) in seconds
   */
  public static double getMatchTime() {
    return DriverStationJNI.getMatchTime();
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
   * Refresh the passed in control word to contain the current control word cache.
   *
   * @param word Word to refresh.
   */
  public static void refreshControlWordFromCache(ControlWord word) {
    m_cacheDataMutex.lock();
    try {
      word.update(m_controlWord);
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Copy data from the DS task for the user. If no new data exists, it will just be returned,
   * otherwise the data will be copied from the DS polling loop.
   */
  public static void refreshData() {
    DriverStationJNI.refreshDSData();

    // Get the status of all the joysticks
    for (byte stick = 0; stick < kJoystickPorts; stick++) {
      m_joystickAxesCache[stick].m_count =
          DriverStationJNI.getJoystickAxes(stick, m_joystickAxesCache[stick].m_axes);
      m_joystickAxesRawCache[stick].m_count =
          DriverStationJNI.getJoystickAxesRaw(stick, m_joystickAxesRawCache[stick].m_axes);
      m_joystickPOVsCache[stick].m_count =
          DriverStationJNI.getJoystickPOVs(stick, m_joystickPOVsCache[stick].m_povs);
      m_joystickButtonsCache[stick].m_buttons =
          DriverStationJNI.getJoystickButtons(stick, m_buttonCountBuffer);
      m_joystickButtonsCache[stick].m_count = m_buttonCountBuffer.get(0);
    }

    DriverStationJNI.getMatchInfo(m_matchInfoCache);

    DriverStationJNI.getControlWord(m_controlWordCache);

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

      HALJoystickAxesRaw[] currentAxesRaw = m_joystickAxesRaw;
      m_joystickAxesRaw = m_joystickAxesRawCache;
      m_joystickAxesRawCache = currentAxesRaw;

      HALJoystickButtons[] currentButtons = m_joystickButtons;
      m_joystickButtons = m_joystickButtonsCache;
      m_joystickButtonsCache = currentButtons;

      HALJoystickPOVs[] currentPOVs = m_joystickPOVs;
      m_joystickPOVs = m_joystickPOVsCache;
      m_joystickPOVsCache = currentPOVs;

      MatchInfoData currentInfo = m_matchInfo;
      m_matchInfo = m_matchInfoCache;
      m_matchInfoCache = currentInfo;

      ControlWord currentWord = m_controlWord;
      m_controlWord = m_controlWordCache;
      m_controlWordCache = currentWord;

      dataLogSender = m_dataLogSender;
    } finally {
      m_cacheDataMutex.unlock();
    }

    m_refreshEvents.wakeup();

    m_matchDataSender.sendMatchData();
    if (dataLogSender != null) {
      dataLogSender.send(WPIUtilJNI.now());
    }
  }

  /**
   * Registers the given handle for DS data refresh notifications.
   *
   * @param handle The event handle.
   */
  public static void provideRefreshedDataEventHandle(int handle) {
    m_refreshEvents.add(handle);
  }

  /**
   * Unregisters the given handle from DS data refresh notifications.
   *
   * @param handle The event handle.
   */
  public static void removeRefreshedDataEventHandle(int handle) {
    m_refreshEvents.remove(handle);
  }

  /**
   * Reports errors related to unplugged joysticks Throttles the errors so that they don't overwhelm
   * the DS.
   */
  private static void reportJoystickUnpluggedError(String message) {
    double currentTime = Timer.getTimestamp();
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
      double currentTime = Timer.getTimestamp();
      if (currentTime > m_nextMessageTime) {
        reportWarning(message, false);
        m_nextMessageTime = currentTime + JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL;
      }
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
