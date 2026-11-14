// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation.internal;

import java.util.Optional;
import java.util.OptionalDouble;
import java.util.concurrent.locks.ReentrantLock;
import org.wpilib.datalog.DataLog;
import org.wpilib.driverstation.MatchState;
import org.wpilib.driverstation.POVDirection;
import org.wpilib.driverstation.RobotState;
import org.wpilib.driverstation.TouchpadFinger;
import org.wpilib.driverstation.internal.JoystickAlerts.JoystickResourceAlert;
import org.wpilib.hardware.hal.AllianceStationID;
import org.wpilib.hardware.hal.ControlWord;
import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.util.WPIUtilJNI;
import org.wpilib.util.concurrent.EventVector;

/** Provide access to the network communication data to / from the Driver Station. */
public final class DriverStationBackend {
  /** Number of Joystick ports. */
  public static final int JOYSTICK_PORTS = 6;

  private static final long[] m_metadataCache = new long[7];
  private static final float[] m_touchpadFingersCache = new float[8];

  static int availableToCount(long available) {
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
    float m_x;
    float m_y;
    boolean m_down;
  }

  private static class HALJoystickTouchpad {
    final HALJoystickTouchpadFinger[] m_fingers =
        new HALJoystickTouchpadFinger[DriverStationJNI.MAX_JOYSTICK_TOUCHPAD_FINGERS];
    int m_count;

    HALJoystickTouchpad() {
      for (int i = 0; i < m_fingers.length; i++) {
        m_fingers[i] = new HALJoystickTouchpadFinger();
      }
    }
  }

  private static class HALJoystickTouchpads {
    final HALJoystickTouchpad[] m_touchpads =
        new HALJoystickTouchpad[DriverStationJNI.MAX_JOYSTICK_TOUCHPADS];
    int m_count;

    HALJoystickTouchpads() {
      for (int i = 0; i < m_touchpads.length; i++) {
        m_touchpads[i] = new HALJoystickTouchpad();
      }
    }
  }

  static final class HALJoystickButtons {
    long m_buttons;
    long m_available;
  }

  static class HALJoystickAxes {
    final float[] m_axes;
    int m_available;

    HALJoystickAxes(int count) {
      m_axes = new float[count];
    }
  }

  private static class HALJoystickAxesRaw {
    final short[] m_axes;

    @SuppressWarnings("unused")
    int m_available;

    HALJoystickAxesRaw(int count) {
      m_axes = new short[count];
    }
  }

  static class HALJoystickPOVs {
    final byte[] m_povs;
    int m_available;

    HALJoystickPOVs(int count) {
      m_povs = new byte[count];
      for (int i = 0; i < count; i++) {
        m_povs[i] = 0;
      }
    }
  }

  // Joystick User Data
  private static HALJoystickAxes[] m_joystickAxes = new HALJoystickAxes[JOYSTICK_PORTS];
  private static HALJoystickAxesRaw[] m_joystickAxesRaw = new HALJoystickAxesRaw[JOYSTICK_PORTS];
  private static HALJoystickPOVs[] m_joystickPOVs = new HALJoystickPOVs[JOYSTICK_PORTS];
  private static HALJoystickButtons[] m_joystickButtons = new HALJoystickButtons[JOYSTICK_PORTS];
  private static HALJoystickTouchpads[] m_joystickTouchpads =
      new HALJoystickTouchpads[JOYSTICK_PORTS];
  private static ControlWord m_controlWord = new ControlWord();
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
  private static String m_gameDataCache = "";
  private static ControlWord m_controlWordCache = new ControlWord();

  // Joystick button rising/falling edge flags
  private static long[] m_joystickButtonsPressed = new long[JOYSTICK_PORTS];
  private static long[] m_joystickButtonsReleased = new long[JOYSTICK_PORTS];

  private static final MatchDataSender m_matchDataSender;
  private static DataLogSender m_dataLogSender;

  private static final ReentrantLock m_cacheDataMutex = new ReentrantLock();

  /**
   * DriverStation constructor.
   *
   * <p>The single DriverStation instance is created statically with the instance static member
   * variable.
   */
  private DriverStationBackend() {}

  static {
    HAL.initialize();

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

    JoystickAlerts.reportJoystickResourceWarning(stick, JoystickResourceAlert.BUTTON, button);
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

    JoystickAlerts.reportJoystickResourceWarning(stick, JoystickResourceAlert.BUTTON, button);
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

    JoystickAlerts.reportJoystickResourceWarning(stick, JoystickResourceAlert.BUTTON, button);
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

    JoystickAlerts.reportJoystickResourceWarning(stick, JoystickResourceAlert.AXIS, axis);
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

    JoystickAlerts.reportJoystickTouchpadFingerWarning(stick, touchpad, finger);
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

    JoystickAlerts.reportJoystickResourceWarning(stick, JoystickResourceAlert.POV, pov);
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
   * Allows the user to specify whether they want joystick connection alerts to be shown. This
   * setting is ignored when the FMS is connected -- alerts will always be on in that scenario.
   *
   * @param silence Whether joystick connection alerts should be silenced.
   */
  public static void silenceJoystickConnectionAlert(boolean silence) {
    JoystickAlerts.silenceJoystickConnectionAlert(silence);
  }

  /**
   * Returns whether joystick connection alerts are silenced. This will always return false when
   * connected to the FMS.
   *
   * @return Whether joystick connection alerts are silenced.
   */
  public static boolean isJoystickConnectionAlertSilenced() {
    return JoystickAlerts.isJoystickConnectionAlertSilenced();
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
  @SuppressWarnings("VariableDeclarationUsageDistance")
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

    // This is a pass through, so if it hasn't changed, it doesn't
    // reallocate
    m_gameDataCache = DriverStationJNI.getGameData(m_gameDataCache);
    MatchState.refreshMatchInfo(m_gameDataCache);

    DriverStationJNI.getControlWord(m_controlWordCache);

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

      ControlWord currentWord = m_controlWord;
      m_controlWord = m_controlWordCache;
      m_controlWordCache = currentWord;

      dataLogSender = m_dataLogSender;
    } finally {
      m_cacheDataMutex.unlock();
    }

    RobotState.refreshData();
    JoystickAlerts.clearJoystickAlerts();

    m_refreshEvents.wakeup();

    m_matchDataSender.sendMatchData(m_controlWord, m_gameDataCache);
    if (dataLogSender != null) {
      dataLogSender.send(
          m_controlWord, m_joystickButtons, m_joystickAxes, m_joystickPOVs, WPIUtilJNI.now());
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
        m_dataLogSender =
            new DataLogSender(
                log,
                logJoysticks,
                m_controlWord,
                m_joystickButtons,
                m_joystickAxes,
                m_joystickPOVs,
                WPIUtilJNI.now());
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
