// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation.internal;

import java.util.HashMap;
import java.util.Map;
import java.util.Optional;
import java.util.OptionalDouble;
import java.util.OptionalInt;
import java.util.concurrent.locks.ReentrantLock;
import java.util.stream.Collectors;
import org.wpilib.datalog.BooleanArrayLogEntry;
import org.wpilib.datalog.DataLog;
import org.wpilib.datalog.FloatArrayLogEntry;
import org.wpilib.datalog.IntegerArrayLogEntry;
import org.wpilib.datalog.StringLogEntry;
import org.wpilib.datalog.StructLogEntry;
import org.wpilib.driverstation.Alliance;
import org.wpilib.driverstation.MatchType;
import org.wpilib.driverstation.POVDirection;
import org.wpilib.driverstation.TouchpadFinger;
import org.wpilib.framework.OpModeRobot;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.hal.AllianceStationID;
import org.wpilib.hardware.hal.ControlWord;
import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.MatchInfoData;
import org.wpilib.hardware.hal.OpModeOption;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.networktables.BooleanPublisher;
import org.wpilib.networktables.IntegerPublisher;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.StringPublisher;
import org.wpilib.networktables.StringTopic;
import org.wpilib.networktables.StructPublisher;
import org.wpilib.system.Timer;
import org.wpilib.util.Color;
import org.wpilib.util.WPIUtilJNI;
import org.wpilib.util.concurrent.EventVector;

/** Provide access to the network communication data to / from the Driver Station. */
public final class DriverStationBackend {
  /** Number of Joystick ports. */
  public static final int JOYSTICK_PORTS = 6;

  private static final long[] m_metadataCache = new long[7];
  private static final float[] m_touchpadFingersCache = new float[8];

  private static int availableToCount(long available) {
    // Top bit has to be set
    if (available < 0) {
      return 64;
    }

    int count = 0;

    // Top bit not set, we will eventually get a 0 bit
    while ((available & 0x1) != 0) {
      count++;
      available >>= 1;
    }
    return count;
  }

  private static final class HALJoystickTouchpadFinger {
    public float m_x;
    public float m_y;
    public boolean m_down;
  }

  private static class HALJoystickTouchpad {
    public final HALJoystickTouchpadFinger[] m_fingers =
        new HALJoystickTouchpadFinger[DriverStationJNI.MAX_JOYSTICK_TOUCHPAD_FINGERS];
    public int m_count;

    HALJoystickTouchpad() {
      for (int i = 0; i < m_fingers.length; i++) {
        m_fingers[i] = new HALJoystickTouchpadFinger();
      }
    }
  }

  private static class HALJoystickTouchpads {
    public final HALJoystickTouchpad[] m_touchpads =
        new HALJoystickTouchpad[DriverStationJNI.MAX_JOYSTICK_TOUCHPADS];
    public int m_count;

    HALJoystickTouchpads() {
      for (int i = 0; i < m_touchpads.length; i++) {
        m_touchpads[i] = new HALJoystickTouchpad();
      }
    }
  }

  private static final class HALJoystickButtons {
    public long m_buttons;
    public long m_available;
  }

  private static class HALJoystickAxes {
    public final float[] m_axes;
    public int m_available;

    HALJoystickAxes(int count) {
      m_axes = new float[count];
    }
  }

  private static class HALJoystickAxesRaw {
    public final short[] m_axes;

    @SuppressWarnings("unused")
    public int m_available;

    HALJoystickAxesRaw(int count) {
      m_axes = new short[count];
    }
  }

  private static class HALJoystickPOVs {
    public final byte[] m_povs;
    public int m_available;

    HALJoystickPOVs(int count) {
      m_povs = new byte[count];
      for (int i = 0; i < count; i++) {
        m_povs[i] = 0;
      }
    }
  }

  private static final double JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL = 1.0;
  private static double m_nextMessageTime;

  private static String opModeToString(long id) {
    if (id == 0) {
      return "";
    }
    m_opModesMutex.lock();
    try {
      OpModeOption option = m_opModes.get(id);
      if (option != null) {
        return option.name;
      }
    } finally {
      m_opModesMutex.unlock();
    }
    return "<" + id + ">";
  }

  @SuppressWarnings("MemberName")
  private static class MatchDataSender {
    private static final String kSmartDashboardType = "FMSInfo";

    final StringPublisher gameData;
    final StringPublisher eventName;
    final IntegerPublisher matchNumber;
    final IntegerPublisher replayNumber;
    final IntegerPublisher matchType;
    final BooleanPublisher alliance;
    final IntegerPublisher station;
    final StructPublisher<ControlWord> controlWord;
    final StringPublisher opMode;
    boolean oldIsRedAlliance = true;
    int oldStationNumber = 1;
    String oldEventName = "";
    String oldGameData = "";
    int oldMatchNumber;
    int oldReplayNumber;
    int oldMatchType;
    final ControlWord oldControlWord = new ControlWord();
    final ControlWord currentControlWord = new ControlWord();

    MatchDataSender() {
      var table = NetworkTableInstance.getDefault().getTable("FMSInfo");
      table
          .getStringTopic(".type")
          .publishEx(
              StringTopic.TYPE_STRING, "{\"SmartDashboard\":\"" + kSmartDashboardType + "\"}")
          .set(kSmartDashboardType);
      gameData = table.getStringTopic("GameData").publish();
      gameData.set("");
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
      controlWord = table.getStructTopic("ControlWord", ControlWord.struct).publish();
      controlWord.set(oldControlWord);
      opMode = table.getStringTopic("OpMode").publish();
      opMode.set("");
    }

    @SuppressWarnings("VariableDeclarationUsageDistance")
    private void sendMatchData() {
      AllianceStationID allianceID = DriverStationJNI.getAllianceStation();
      final int stationNumber =
          switch (allianceID) {
            case BLUE_1, RED_1 -> 1;
            case BLUE_2, RED_2 -> 2;
            case BLUE_3, RED_3, UNKNOWN -> 3;
          };
      final boolean isRedAlliance =
          switch (allianceID) {
            case BLUE_1, BLUE_2, BLUE_3 -> false;
            case RED_1, RED_2, RED_3, UNKNOWN -> true;
          };

      String currentEventName;
      String currentGameData;
      int currentMatchNumber;
      int currentReplayNumber;
      int currentMatchType;
      m_cacheDataMutex.lock();
      try {
        currentEventName = DriverStationBackend.m_matchInfo.eventName;
        currentGameData = DriverStationBackend.m_gameData;
        currentMatchNumber = DriverStationBackend.m_matchInfo.matchNumber;
        currentReplayNumber = DriverStationBackend.m_matchInfo.replayNumber;
        currentMatchType = DriverStationBackend.m_matchInfo.matchType;
      } finally {
        m_cacheDataMutex.unlock();
      }
      DriverStationJNI.getControlWord(currentControlWord);

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
      if (!oldGameData.equals(currentGameData)) {
        gameData.set(currentGameData);
        oldGameData = currentGameData;
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
      if (!currentControlWord.equals(oldControlWord)) {
        long currentOpModeId = currentControlWord.getOpModeId();
        if (currentOpModeId != oldControlWord.getOpModeId()) {
          opMode.set(opModeToString(currentOpModeId));
        }
        controlWord.set(currentControlWord);
        oldControlWord.update(currentControlWord);
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
      if (buttons.m_available != m_prevButtons.m_available
          || buttons.m_buttons != m_prevButtons.m_buttons) {
        appendButtons(buttons, timestamp);
      }

      HALJoystickAxes axes = m_joystickAxes[m_stick];
      int available = axes.m_available;
      boolean needToLog = false;
      if (available != m_prevAxes.m_available) {
        needToLog = true;
      } else {
        for (int i = 0; i < m_prevAxes.m_axes.length; i++) {
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
      available = m_joystickPOVs[m_stick].m_available;
      needToLog = false;
      if (available != m_prevPOVs.m_available) {
        needToLog = true;
      } else {
        for (int i = 0; i < m_prevPOVs.m_povs.length; i++) {
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
      int count = availableToCount(buttons.m_available);
      if (m_sizedButtons == null || m_sizedButtons.length != count) {
        m_sizedButtons = new boolean[count];
      }
      long buttonsValue = buttons.m_buttons;
      for (int i = 0; i < count; i++) {
        m_sizedButtons[i] = (buttonsValue & (1L << i)) != 0;
      }
      m_logButtons.append(m_sizedButtons, timestamp);
      m_prevButtons.m_available = buttons.m_available;
      m_prevButtons.m_buttons = buttons.m_buttons;
    }

    void appendAxes(HALJoystickAxes axes, long timestamp) {
      int count = availableToCount(axes.m_available);
      if (m_sizedAxes == null || m_sizedAxes.length != count) {
        m_sizedAxes = new float[count];
      }
      System.arraycopy(axes.m_axes, 0, m_sizedAxes, 0, count);
      m_logAxes.append(m_sizedAxes, timestamp);
      m_prevAxes.m_available = axes.m_available;
      System.arraycopy(axes.m_axes, 0, m_prevAxes.m_axes, 0, count);
    }

    void appendPOVs(HALJoystickPOVs povs, long timestamp) {
      int count = availableToCount(povs.m_available);
      if (m_sizedPOVs == null || m_sizedPOVs.length != count) {
        m_sizedPOVs = new long[count];
      }
      for (int i = 0; i < count; i++) {
        m_sizedPOVs[i] = povs.m_povs[i];
      }
      m_logPOVs.append(m_sizedPOVs, timestamp);
      m_prevPOVs.m_available = povs.m_available;
      System.arraycopy(povs.m_povs, 0, m_prevPOVs.m_povs, 0, count);
    }

    final int m_stick;
    boolean[] m_sizedButtons;
    float[] m_sizedAxes;
    long[] m_sizedPOVs;
    final HALJoystickButtons m_prevButtons = new HALJoystickButtons();
    final HALJoystickAxes m_prevAxes = new HALJoystickAxes(DriverStationJNI.MAX_JOYSTICK_AXES);
    final HALJoystickPOVs m_prevPOVs = new HALJoystickPOVs(DriverStationJNI.MAX_JOYSTICK_POVS);
    final BooleanArrayLogEntry m_logButtons;
    final FloatArrayLogEntry m_logAxes;
    final IntegerArrayLogEntry m_logPOVs;
  }

  private static class DataLogSender {
    DataLogSender(DataLog log, boolean logJoysticks, long timestamp) {
      m_logControlWord =
          StructLogEntry.create(log, "DS:controlWord", ControlWord.struct, timestamp);

      // append initial control word value
      m_logControlWord.append(m_controlWordCache, timestamp);
      m_oldControlWord.update(m_controlWordCache);

      // append initial opmode value
      m_logOpMode = new StringLogEntry(log, "DS:opMode", timestamp);
      m_logOpMode.append(m_opModeCache, timestamp);

      if (logJoysticks) {
        m_joysticks = new JoystickLogSender[JOYSTICK_PORTS];
        for (int i = 0; i < JOYSTICK_PORTS; i++) {
          m_joysticks[i] = new JoystickLogSender(log, i, timestamp);
        }
      } else {
        m_joysticks = new JoystickLogSender[0];
      }
    }

    public void send(long timestamp) {
      // append control word value changes
      if (!m_controlWordCache.equals(m_oldControlWord)) {
        // append opmode value changes
        long opModeId = m_controlWordCache.getOpModeId();
        if (opModeId != m_oldControlWord.getOpModeId()) {
          m_logOpMode.append(m_opModeCache, timestamp);
        }

        m_logControlWord.append(m_controlWordCache, timestamp);
        m_oldControlWord.update(m_controlWordCache);
      }

      // append joystick value changes
      for (JoystickLogSender joystick : m_joysticks) {
        joystick.send(timestamp);
      }
    }

    final ControlWord m_oldControlWord = new ControlWord();
    final StructLogEntry<ControlWord> m_logControlWord;
    final StringLogEntry m_logOpMode;

    final JoystickLogSender[] m_joysticks;
  }

  // Joystick User Data
  private static HALJoystickAxes[] m_joystickAxes = new HALJoystickAxes[JOYSTICK_PORTS];
  private static HALJoystickAxesRaw[] m_joystickAxesRaw = new HALJoystickAxesRaw[JOYSTICK_PORTS];
  private static HALJoystickPOVs[] m_joystickPOVs = new HALJoystickPOVs[JOYSTICK_PORTS];
  private static HALJoystickButtons[] m_joystickButtons = new HALJoystickButtons[JOYSTICK_PORTS];
  private static HALJoystickTouchpads[] m_joystickTouchpads =
      new HALJoystickTouchpads[JOYSTICK_PORTS];
  private static MatchInfoData m_matchInfo = new MatchInfoData();
  private static String m_gameData = "";
  private static ControlWord m_controlWord = new ControlWord();
  private static String m_opMode = "";
  private static EventVector m_refreshEvents = new EventVector();

  // Joystick Cached Data
  private static HALJoystickAxes[] m_joystickAxesCache = new HALJoystickAxes[JOYSTICK_PORTS];
  private static HALJoystickAxesRaw[] m_joystickAxesRawCache =
      new HALJoystickAxesRaw[JOYSTICK_PORTS];
  private static HALJoystickPOVs[] m_joystickPOVsCache = new HALJoystickPOVs[JOYSTICK_PORTS];
  private static HALJoystickButtons[] m_joystickButtonsCache =
      new HALJoystickButtons[JOYSTICK_PORTS];
  private static HALJoystickTouchpads[] m_joystickTouchpadsCache =
      new HALJoystickTouchpads[JOYSTICK_PORTS];
  private static MatchInfoData m_matchInfoCache = new MatchInfoData();
  private static String m_gameDataCache = "";
  private static ControlWord m_controlWordCache = new ControlWord();

  private static String m_opModeCache = "";

  // Joystick button rising/falling edge flags
  private static long[] m_joystickButtonsPressed = new long[JOYSTICK_PORTS];
  private static long[] m_joystickButtonsReleased = new long[JOYSTICK_PORTS];

  private static final MatchDataSender m_matchDataSender;
  private static DataLogSender m_dataLogSender;

  private static final ReentrantLock m_cacheDataMutex = new ReentrantLock();

  private static boolean m_silenceJoystickWarning;

  private static boolean m_userProgramStarted = false;
  private static final Map<Long, OpModeOption> m_opModes = new HashMap<>();
  private static final ReentrantLock m_opModesMutex = new ReentrantLock();

  /**
   * DriverStation constructor.
   *
   * <p>The single DriverStation instance is created statically with the instance static member
   * variable.
   */
  private DriverStationBackend() {}

  static {
    HAL.initialize(500, 0);

    for (int i = 0; i < JOYSTICK_PORTS; i++) {
      m_joystickButtons[i] = new HALJoystickButtons();
      m_joystickAxes[i] = new HALJoystickAxes(DriverStationJNI.MAX_JOYSTICK_AXES);
      m_joystickAxesRaw[i] = new HALJoystickAxesRaw(DriverStationJNI.MAX_JOYSTICK_AXES);
      m_joystickPOVs[i] = new HALJoystickPOVs(DriverStationJNI.MAX_JOYSTICK_POVS);
      m_joystickTouchpads[i] = new HALJoystickTouchpads();

      m_joystickButtonsCache[i] = new HALJoystickButtons();
      m_joystickAxesCache[i] = new HALJoystickAxes(DriverStationJNI.MAX_JOYSTICK_AXES);
      m_joystickAxesRawCache[i] = new HALJoystickAxesRaw(DriverStationJNI.MAX_JOYSTICK_AXES);
      m_joystickPOVsCache[i] = new HALJoystickPOVs(DriverStationJNI.MAX_JOYSTICK_POVS);
      m_joystickTouchpadsCache[i] = new HALJoystickTouchpads();
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
        if (!haveLoc && !loc.startsWith("org.wpilib")) {
          locString = loc;
          haveLoc = true;
        }
      }
    }
    DriverStationJNI.sendError(
        isError, code, false, error, locString, traceString.toString(), true);
  }

  /**
   * The state of one joystick button.
   *
   * @param stick The joystick to read.
   * @param button The button index.
   * @return The state of the joystick button.
   */
  public static boolean getStickButton(final int stick, final int button) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }
    if (button < 0 || button >= DriverStationJNI.MAX_JOYSTICK_BUTTONS) {
      throw new IllegalArgumentException("Joystick Button is out of range");
    }

    long mask = 1L << button;

    m_cacheDataMutex.lock();
    try {
      if ((m_joystickButtons[stick].m_available & mask) != 0) {
        return (m_joystickButtons[stick].m_buttons & mask) != 0;
      }
    } finally {
      m_cacheDataMutex.unlock();
    }

    reportJoystickWarning(
        stick, "Joystick Button " + button + " on port " + stick + " not available");
    return false;
  }

  /**
   * The state of one joystick button if available.
   *
   * @param stick The joystick to read.
   * @param button The button index.
   * @return The state of the joystick button, or false if the button is not available.
   */
  public static Optional<Boolean> getStickButtonIfAvailable(final int stick, final int button) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }
    if (button < 0 || button >= DriverStationJNI.MAX_JOYSTICK_BUTTONS) {
      throw new IllegalArgumentException("Joystick Button is out of range");
    }

    long mask = 1L << button;

    m_cacheDataMutex.lock();
    try {
      if ((m_joystickButtons[stick].m_available & mask) != 0) {
        return Optional.of((m_joystickButtons[stick].m_buttons & mask) != 0);
      }
    } finally {
      m_cacheDataMutex.unlock();
    }
    return Optional.empty();
  }

  /**
   * Whether one joystick button was pressed since the last check.
   *
   * @param stick The joystick to read.
   * @param button The button index.
   * @return Whether the joystick button was pressed since the last check.
   */
  public static boolean getStickButtonPressed(final int stick, final int button) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }
    if (button < 0 || button >= DriverStationJNI.MAX_JOYSTICK_BUTTONS) {
      throw new IllegalArgumentException("Joystick Button is out of range");
    }

    long mask = 1L << button;

    m_cacheDataMutex.lock();
    try {
      if ((m_joystickButtons[stick].m_available & mask) != 0) {
        // If button was pressed, clear flag and return true
        if ((m_joystickButtonsPressed[stick] & mask) != 0) {
          m_joystickButtonsPressed[stick] &= ~mask;
          return true;
        } else {
          return false;
        }
      }
    } finally {
      m_cacheDataMutex.unlock();
    }

    reportJoystickWarning(
        stick, "Joystick Button " + button + " on port " + stick + " not available");
    return false;
  }

  /**
   * Whether one joystick button was released since the last check.
   *
   * @param stick The joystick to read.
   * @param button The button index, beginning at 0.
   * @return Whether the joystick button was released since the last check.
   */
  public static boolean getStickButtonReleased(final int stick, final int button) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }
    if (button < 0 || button >= DriverStationJNI.MAX_JOYSTICK_BUTTONS) {
      throw new IllegalArgumentException("Joystick Button is out of range");
    }

    long mask = 1L << button;

    m_cacheDataMutex.lock();
    try {
      if ((m_joystickButtons[stick].m_available & mask) != 0) {
        // If button was released, clear flag and return true
        if ((m_joystickButtonsReleased[stick] & mask) != 0) {
          m_joystickButtonsReleased[stick] &= ~mask;
          return true;
        } else {
          return false;
        }
      }
    } finally {
      m_cacheDataMutex.unlock();
    }

    reportJoystickWarning(
        stick, "Joystick Button " + button + " on port " + stick + " not available");
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
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }
    if (axis < 0 || axis >= DriverStationJNI.MAX_JOYSTICK_AXES) {
      throw new IllegalArgumentException("Joystick axis is out of range");
    }

    int mask = 1 << axis;

    m_cacheDataMutex.lock();
    try {
      if ((m_joystickAxes[stick].m_available & mask) != 0) {
        return m_joystickAxes[stick].m_axes[axis];
      }
    } finally {
      m_cacheDataMutex.unlock();
    }

    reportJoystickWarning(stick, "Joystick axis " + axis + " on port " + stick + " not available");
    return 0.0;
  }

  /**
   * Get the state of a touchpad finger on the joystick.
   *
   * @param stick The joystick to read.
   * @param touchpad The touchpad to read.
   * @param finger The finger to read.
   * @return the state of the touchpad finger.
   */
  public static TouchpadFinger getStickTouchpadFinger(int stick, int touchpad, int finger) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }
    if (touchpad < 0 || touchpad >= DriverStationJNI.MAX_JOYSTICK_TOUCHPADS) {
      throw new IllegalArgumentException("Joystick touchpad is out of range");
    }
    if (finger < 0 || finger >= DriverStationJNI.MAX_JOYSTICK_TOUCHPAD_FINGERS) {
      throw new IllegalArgumentException("Joystick touchpad finger is out of range");
    }

    int touchpadCount;
    m_cacheDataMutex.lock();
    try {
      touchpadCount = m_joystickTouchpads[stick].m_count;
      if (touchpad < touchpadCount) {
        HALJoystickTouchpad tp = m_joystickTouchpads[stick].m_touchpads[touchpad];
        if (finger < tp.m_count) {
          return new TouchpadFinger(
              tp.m_fingers[finger].m_down, tp.m_fingers[finger].m_x, tp.m_fingers[finger].m_y);
        }
      }
    } finally {
      m_cacheDataMutex.unlock();
    }

    reportJoystickWarning(
        stick,
        "Joystick touchpad finger "
            + finger
            + " on touchpad "
            + touchpad
            + " on port "
            + stick
            + " not available");
    return new TouchpadFinger(false, 0.0f, 0.0f);
  }

  /**
   * Get whether a touchpad finger on the joystick is available.
   *
   * @param stick The joystick to read.
   * @param touchpad The touchpad to read.
   * @param finger The finger to read.
   * @return whether the touchpad finger is available.
   */
  public static boolean getStickTouchpadFingerAvailable(int stick, int touchpad, int finger) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }
    if (touchpad < 0 || touchpad >= DriverStationJNI.MAX_JOYSTICK_TOUCHPADS) {
      throw new IllegalArgumentException("Joystick touchpad is out of range");
    }
    if (finger < 0 || finger >= DriverStationJNI.MAX_JOYSTICK_TOUCHPAD_FINGERS) {
      throw new IllegalArgumentException("Joystick touchpad finger is out of range");
    }

    int touchpadCount;
    m_cacheDataMutex.lock();
    try {
      touchpadCount = m_joystickTouchpads[stick].m_count;
      if (touchpad < touchpadCount) {
        HALJoystickTouchpad tp = m_joystickTouchpads[stick].m_touchpads[touchpad];
        if (finger < tp.m_count) {
          return true;
        }
      }
    } finally {
      m_cacheDataMutex.unlock();
    }

    return false;
  }

  /**
   * Get the value of the axis on a joystick if available. This depends on the mapping of the
   * joystick connected to the specified port.
   *
   * @param stick The joystick to read.
   * @param axis The analog axis value to read from the joystick.
   * @return The value of the axis on the joystick, or 0 if the axis is not available.
   */
  public static OptionalDouble getStickAxisIfAvailable(int stick, int axis) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }
    if (axis < 0 || axis >= DriverStationJNI.MAX_JOYSTICK_AXES) {
      throw new IllegalArgumentException("Joystick axis is out of range");
    }

    int mask = 1 << axis;

    m_cacheDataMutex.lock();
    try {
      if ((m_joystickAxes[stick].m_available & mask) != 0) {
        return OptionalDouble.of(m_joystickAxes[stick].m_axes[axis]);
      }
    } finally {
      m_cacheDataMutex.unlock();
    }

    return OptionalDouble.empty();
  }

  /**
   * Get the state of a POV on the joystick.
   *
   * @param stick The joystick to read.
   * @param pov The POV to read.
   * @return the angle of the POV.
   */
  public static POVDirection getStickPOV(int stick, int pov) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }
    if (pov < 0 || pov >= DriverStationJNI.MAX_JOYSTICK_POVS) {
      throw new IllegalArgumentException("Joystick POV is out of range");
    }

    int mask = 1 << pov;

    m_cacheDataMutex.lock();
    try {
      if ((m_joystickPOVs[stick].m_available & mask) != 0) {
        return POVDirection.of(m_joystickPOVs[stick].m_povs[pov]);
      }
    } finally {
      m_cacheDataMutex.unlock();
    }

    reportJoystickWarning(stick, "Joystick POV " + pov + " on port " + stick + " not available");
    return POVDirection.CENTER;
  }

  /**
   * The state of the buttons on the joystick.
   *
   * @param stick The joystick to read.
   * @return The state of the buttons on the joystick.
   */
  public static long getStickButtons(final int stick) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
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
   * Gets the maximum index of axes on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The maximum index of axes on the indicated joystick
   */
  public static int getStickAxesMaximumIndex(int stick) {
    return availableToCount(getStickAxesAvailable(stick));
  }

  /**
   * Returns the available bitmask of axes on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The number of axes available on the indicated joystick
   */
  public static int getStickAxesAvailable(int stick) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    m_cacheDataMutex.lock();
    try {
      return m_joystickAxes[stick].m_available;
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets the maximum index of povs on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The maximum index of povs on the indicated joystick
   */
  public static int getStickPOVsMaximumIndex(int stick) {
    return availableToCount(getStickPOVsAvailable(stick));
  }

  /**
   * Returns the available bitmask of povs on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The number of povs available on the indicated joystick
   */
  public static int getStickPOVsAvailable(int stick) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    m_cacheDataMutex.lock();
    try {
      return m_joystickPOVs[stick].m_available;
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets the maximum index of buttons on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The maximum index of buttons on the indicated joystick
   */
  public static int getStickButtonsMaximumIndex(int stick) {
    return availableToCount(getStickButtonsAvailable(stick));
  }

  /**
   * Gets the bitmask of buttons available.
   *
   * @param stick The joystick port number
   * @return The buttons available on the indicated joystick
   */
  public static long getStickButtonsAvailable(int stick) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    m_cacheDataMutex.lock();
    try {
      return m_joystickButtons[stick].m_available;
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets the value of isGamepad on a joystick.
   *
   * @param stick The joystick port number
   * @return A boolean that returns the value of isGamepad
   */
  public static boolean getJoystickIsGamepad(int stick) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    return DriverStationJNI.getJoystickIsGamepad((byte) stick) == 1;
  }

  /**
   * Gets the value of type on a gamepad.
   *
   * @param stick The joystick port number
   * @return The value of type
   */
  public static int getJoystickGamepadType(int stick) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    return DriverStationJNI.getJoystickGamepadType((byte) stick);
  }

  /**
   * Gets the value of supported outputs on a joystick.
   *
   * @param stick The joystick port number
   * @return The value of supported outputs
   */
  public static int getJoystickSupportedOutputs(int stick) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    return DriverStationJNI.getJoystickSupportedOutputs((byte) stick);
  }

  /**
   * Gets the name of the joystick at a port.
   *
   * @param stick The joystick port number
   * @return The value of name
   */
  public static String getJoystickName(int stick) {
    if (stick < 0 || stick >= JOYSTICK_PORTS) {
      throw new IllegalArgumentException("Joystick index is out of range, should be 0-5");
    }

    return DriverStationJNI.getJoystickName((byte) stick);
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
    return getStickAxesAvailable(stick) != 0
        || getStickButtonsAvailable(stick) != 0
        || getStickPOVsAvailable(stick) != 0;
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be enabled.
   *
   * @return True if the robot is enabled, false otherwise.
   */
  public static boolean isEnabled() {
    m_cacheDataMutex.lock();
    try {
      return m_controlWord.isEnabled() && m_controlWord.isDSAttached();
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
      return m_controlWord.isEStopped();
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets the current robot mode.
   *
   * <p>Note that this does not indicate whether the robot is enabled or disabled.
   *
   * @return robot mode
   */
  public static RobotMode getRobotMode() {
    m_cacheDataMutex.lock();
    try {
      return m_controlWord.getRobotMode();
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
      return m_controlWord.isAutonomous();
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
      return m_controlWord.isAutonomousEnabled();
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
    return m_controlWord.isTeleop();
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
      return m_controlWord.isTeleopEnabled();
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in Utility
   * mode.
   *
   * @return True if utility mode should be enabled, false otherwise.
   */
  public static boolean isUtility() {
    m_cacheDataMutex.lock();
    try {
      return m_controlWord.isUtility();
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in Utility
   * mode and enabled.
   *
   * @return True if utility mode should be set and the robot should be enabled.
   */
  public static boolean isUtilityEnabled() {
    m_cacheDataMutex.lock();
    try {
      return m_controlWord.isUtilityEnabled();
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  private static int convertColorToInt(Color color) {
    if (color == null) {
      return -1;
    }
    return (((int) (color.red * 255) & 0xff) << 16)
        | (((int) (color.green * 255) & 0xff) << 8)
        | ((int) (color.blue * 255) & 0xff);
  }

  /**
   * Adds an operating mode option. It's necessary to call publishOpModes() to make the added modes
   * visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   * @param textColor text color, or null for default
   * @param backgroundColor background color, or null for default
   * @return unique ID used to later identify the operating mode
   * @throws IllegalArgumentException if name is empty or an operating mode with the same robot mode
   *     and name already exists
   */
  @SuppressWarnings("PMD.UseStringBufferForStringAppends")
  public static long addOpMode(
      RobotMode mode,
      String name,
      String group,
      String description,
      Color textColor,
      Color backgroundColor) {
    if (name.isBlank()) {
      throw new IllegalArgumentException("OpMode name must be non-blank");
    }
    // find unique ID
    m_opModesMutex.lock();
    try {
      String nameCopy = name;
      for (; ; ) {
        long id = OpModeOption.makeId(mode, nameCopy.hashCode());
        OpModeOption existing = m_opModes.get(id);
        if (existing == null) {
          m_opModes.put(
              id,
              new OpModeOption(
                  id,
                  name,
                  group,
                  description,
                  convertColorToInt(textColor),
                  convertColorToInt(backgroundColor)));
          return id;
        }
        if (existing.getMode() == mode && existing.name.equals(name)) {
          // already exists
          throw new IllegalArgumentException("OpMode " + name + " already exists for mode " + mode);
        }
        // collision, try again with space appended
        nameCopy += ' ';
      }
    } finally {
      m_opModesMutex.unlock();
    }
  }

  /**
   * Adds an operating mode option. It's necessary to call publishOpModes() to make the added modes
   * visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   * @return unique ID used to later identify the operating mode
   * @throws IllegalArgumentException if name is empty or an operating mode with the same name
   *     already exists
   */
  public static long addOpMode(RobotMode mode, String name, String group, String description) {
    return addOpMode(mode, name, group, description, null, null);
  }

  /**
   * Adds an operating mode option. It's necessary to call publishOpModes() to make the added modes
   * visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @return unique ID used to later identify the operating mode
   * @throws IllegalArgumentException if name is empty or an operating mode with the same name
   *     already exists
   */
  public static long addOpMode(RobotMode mode, String name, String group) {
    return addOpMode(mode, name, group, "");
  }

  /**
   * Adds an operating mode option. It's necessary to call publishOpModes() to make the added modes
   * visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   * @return unique ID used to later identify the operating mode
   * @throws IllegalArgumentException if name is empty or an operating mode with the same name
   *     already exists
   */
  public static long addOpMode(RobotMode mode, String name) {
    return addOpMode(mode, name, "");
  }

  /**
   * Removes an operating mode option. It's necessary to call publishOpModes() to make the removed
   * mode no longer visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   * @return unique ID for the opmode, or 0 if not found
   */
  public static long removeOpMode(RobotMode mode, String name) {
    if (name.isBlank()) {
      return 0;
    }
    m_opModesMutex.lock();
    try {
      // we have to loop over all entries to find the one with the correct name
      // because the of the unique ID generation scheme
      for (OpModeOption opMode : m_opModes.values()) {
        if (opMode.getMode() == mode && opMode.name.equals(name)) {
          m_opModes.remove(opMode.id);
          return opMode.id;
        }
      }
    } finally {
      m_opModesMutex.unlock();
    }
    return 0;
  }

  /** Publishes the operating mode options to the driver station. */
  public static void publishOpModes() {
    m_opModesMutex.lock();
    try {
      OpModeOption[] options = new OpModeOption[m_opModes.size()];
      DriverStationJNI.setOpModeOptions(m_opModes.values().toArray(options));
    } finally {
      m_opModesMutex.unlock();
    }

    var modeCounts =
        m_opModes.values().stream()
            .collect(Collectors.groupingBy(OpModeOption::getMode, Collectors.counting()));

    for (RobotMode mode : RobotMode.values()) {
      HAL.reportUsage("OpMode/" + mode, String.valueOf(modeCounts.getOrDefault(mode, 0L)));
    }
  }

  /** Clears all operating mode options and publishes an empty list to the driver station. */
  public static void clearOpModes() {
    m_opModesMutex.lock();
    try {
      m_opModes.clear();
      DriverStationJNI.setOpModeOptions(new OpModeOption[0]);
    } finally {
      m_opModesMutex.unlock();
    }
  }

  /**
   * Sets the program starting flag in the DS. This will also allow {@link #getOpModeId()} and
   * {@link #getOpMode()} to return values for the selected OpMode in the DS application, if the DS
   * is connected by the time this method is called.
   *
   * <p>Most users will not need to use this method; the {@link TimedRobot} and {@link OpModeRobot}
   * robot framework classes will call it automatically after the main robot class is instantiated.
   * However, teams using the commandsv3 library and a custom main robot class need to be careful to
   * only call this method after all mechanisms and global trigger bindings are set up. If not, any
   * setup performed in the main robot class may be incorrectly bound to the opmode selected in the
   * DS if it's connected by the time the robot program boots up.
   *
   * <p>This is what changes the DS to showing robot code ready.
   *
   * @see #getOpMode()
   * @see #getOpModeId()
   */
  public static void observeUserProgramStarting() {
    m_userProgramStarted = true;
    DriverStationJNI.observeUserProgramStarting();
  }

  /**
   * Gets the operating mode selected on the driver station. Note this does not mean the robot is
   * enabled; use isEnabled() for that. In a match, this will indicate the operating mode selected
   * for auto before the match starts (i.e., while the robot is disabled in auto mode); after the
   * auto period ends, this will change to reflect the operating mode selected for teleop.
   *
   * <p>This method always returns {@code 0} while the main robot class is being constructed and
   * initialized (more specifically, it returns {@code 0} until {@link
   * #observeUserProgramStarting()} is called, which the WPILib framework will automatically call
   * during {@link TimedRobot#startCompetition()} and {@link OpModeRobot#startCompetition()}).
   *
   * @return the unique ID provided by the addOpMode() function; may return 0 or a unique ID not
   *     added, so callers should be prepared to handle that case
   */
  public static long getOpModeId() {
    if (!m_userProgramStarted) {
      return 0;
    }

    m_cacheDataMutex.lock();
    try {
      return m_controlWord.getOpModeId();
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Gets the operating mode selected on the driver station. Note this does not mean the robot is
   * enabled; use isEnabled() for that. In a match, this will indicate the operating mode selected
   * for auto before the match starts (i.e., while the robot is disabled in auto mode); after the
   * auto period ends, this will change to reflect the operating mode selected for teleop.
   *
   * <p>This method always returns an empty string {@code ""} while the main robot class is being
   * constructed and initialized (more specifically, it returns {@code ""} until {@link
   * #observeUserProgramStarting()} is called, which the WPILib framework will automatically call
   * during {@link TimedRobot#startCompetition()} and {@link OpModeRobot#startCompetition()}).
   *
   * @return Operating mode string; may return a string not in the list of options, so callers
   *     should be prepared to handle that case
   */
  public static String getOpMode() {
    if (!m_userProgramStarted) {
      return "";
    }

    m_cacheDataMutex.lock();
    try {
      return m_opMode;
    } finally {
      m_cacheDataMutex.unlock();
    }
  }

  /**
   * Check to see if the selected operating mode is a particular value. Note this does not mean the
   * robot is enabled; use isEnabled() for that.
   *
   * @param id operating mode unique ID
   * @return True if that mode is the current mode
   */
  public static boolean isOpMode(long id) {
    return getOpModeId() == id;
  }

  /**
   * Check to see if the selected operating mode is a particular value. Note this does not mean the
   * robot is enabled; use isEnabled() for that.
   *
   * @param mode operating mode
   * @return True if that mode is the current mode
   */
  public static boolean isOpMode(String mode) {
    return getOpMode().equals(mode);
  }

  /**
   * Gets a value indicating whether the Driver Station is attached.
   *
   * @return True if Driver Station is attached, false otherwise.
   */
  public static boolean isDSAttached() {
    m_cacheDataMutex.lock();
    try {
      return m_controlWord.isDSAttached();
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
      return m_controlWord.isFMSAttached();
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
  public static Optional<String> getGameData() {
    m_cacheDataMutex.lock();
    try {
      if (m_gameData == null || m_gameData.isEmpty()) {
        return Optional.empty();
      }
      return Optional.of(m_gameData);
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
      case 1 -> MatchType.PRACTICE;
      case 2 -> MatchType.QUALIFICATION;
      case 3 -> MatchType.ELIMINATION;
      default -> MatchType.NONE;
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
          AllianceStationID.UNKNOWN, Optional.empty(),
          AllianceStationID.RED_1, Optional.of(Alliance.RED),
          AllianceStationID.RED_2, Optional.of(Alliance.RED),
          AllianceStationID.RED_3, Optional.of(Alliance.RED),
          AllianceStationID.BLUE_1, Optional.of(Alliance.BLUE),
          AllianceStationID.BLUE_2, Optional.of(Alliance.BLUE),
          AllianceStationID.BLUE_3, Optional.of(Alliance.BLUE));

  private static Map<AllianceStationID, OptionalInt> m_stationMap =
      Map.of(
          AllianceStationID.UNKNOWN, OptionalInt.empty(),
          AllianceStationID.RED_1, OptionalInt.of(1),
          AllianceStationID.RED_2, OptionalInt.of(2),
          AllianceStationID.RED_3, OptionalInt.of(3),
          AllianceStationID.BLUE_1, OptionalInt.of(1),
          AllianceStationID.BLUE_2, OptionalInt.of(2),
          AllianceStationID.BLUE_3, OptionalInt.of(3));

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
      allianceStationID = AllianceStationID.UNKNOWN;
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
      allianceStationID = AllianceStationID.UNKNOWN;
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
   * <p>When the DS is in teleop or autonomous mode, this number returns -1.0.
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
    for (byte stick = 0; stick < JOYSTICK_PORTS; stick++) {
      DriverStationJNI.getAllJoystickData(
          stick,
          m_joystickAxesCache[stick].m_axes,
          m_joystickAxesRawCache[stick].m_axes,
          m_joystickPOVsCache[stick].m_povs,
          m_touchpadFingersCache,
          m_metadataCache);
      m_joystickAxesCache[stick].m_available = (int) m_metadataCache[0];
      m_joystickAxesRawCache[stick].m_available = (int) m_metadataCache[0];
      m_joystickPOVsCache[stick].m_available = (int) m_metadataCache[1];
      m_joystickButtonsCache[stick].m_available = m_metadataCache[2];
      m_joystickButtonsCache[stick].m_buttons = m_metadataCache[3];
      m_joystickTouchpadsCache[stick].m_count = (int) m_metadataCache[4];
      for (int i = 0; i < m_joystickTouchpadsCache[stick].m_count; i++) {
        long metadata = m_metadataCache[5 + i];
        m_joystickTouchpadsCache[stick].m_touchpads[i].m_fingers[0].m_down = (metadata & 0x1) != 0;
        m_joystickTouchpadsCache[stick].m_touchpads[i].m_fingers[1].m_down = (metadata & 0x2) != 0;
        m_joystickTouchpadsCache[stick].m_touchpads[i].m_count = (int) (metadata >> 2 & 0x3);
        for (int j = 0; j < m_joystickTouchpadsCache[stick].m_touchpads[i].m_count; j++) {
          m_joystickTouchpadsCache[stick].m_touchpads[i].m_fingers[j].m_x =
              m_touchpadFingersCache[i * 4 + j * 2 + 0];
          m_joystickTouchpadsCache[stick].m_touchpads[i].m_fingers[j].m_y =
              m_touchpadFingersCache[i * 4 + j * 2 + 1];
        }
      }
    }

    DriverStationJNI.getMatchInfo(m_matchInfoCache);

    // This is a pass through, so if it hasn't changed, it doesn't
    // reallocate
    m_gameDataCache = DriverStationJNI.getGameData(m_gameDataCache);

    DriverStationJNI.getControlWord(m_controlWordCache);

    m_opModeCache = opModeToString(m_controlWordCache.getOpModeId());

    DataLogSender dataLogSender;
    // lock joystick mutex to swap cache data
    m_cacheDataMutex.lock();
    try {
      for (int i = 0; i < JOYSTICK_PORTS; i++) {
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

      HALJoystickTouchpads[] currentTouchpads = m_joystickTouchpads;
      m_joystickTouchpads = m_joystickTouchpadsCache;
      m_joystickTouchpadsCache = currentTouchpads;

      MatchInfoData currentInfo = m_matchInfo;
      m_matchInfo = m_matchInfoCache;
      m_matchInfoCache = currentInfo;

      m_gameData = m_gameDataCache;

      ControlWord currentWord = m_controlWord;
      m_controlWord = m_controlWordCache;
      m_controlWordCache = currentWord;

      String currentOpMode = m_opMode;
      m_opMode = m_opModeCache;
      m_opModeCache = currentOpMode;

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
   * Reports errors related to joystick availability. Throttles the errors so that they don't
   * overwhelm the DS.
   *
   * @param stick The joystick port.
   * @param message The message to report if the joystick is connected.
   */
  private static void reportJoystickWarning(int stick, String message) {
    if (isFMSAttached() || !m_silenceJoystickWarning) {
      double currentTime = Timer.getTimestamp();
      if (currentTime > m_nextMessageTime) {
        if (isJoystickConnected(stick)) {
          reportWarning(message, false);
        } else {
          reportWarning(
              "Joystick on port " + stick + " not available, check if controller is plugged in",
              false);
        }
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
