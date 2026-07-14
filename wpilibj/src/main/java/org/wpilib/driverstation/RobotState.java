// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.locks.ReentrantLock;
import java.util.stream.Collectors;
import org.wpilib.driverstation.internal.DriverStationBackend;
import org.wpilib.framework.OpModeRobot;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.hal.ControlWord;
import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.OpModeOption;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.util.Color;

/** Provides access to robot state information from the Driver Station. */
public final class RobotState {
  static {
    HAL.initialize(500, 0);
  }

  private static ControlWord m_controlWord = new ControlWord();
  private static ControlWord m_controlWordCache = new ControlWord();
  private static final ReentrantLock m_robotStateMutex = new ReentrantLock();
  private static boolean m_userProgramStarted = false;
  private static String m_opMode = "";
  private static final Map<Long, OpModeOption> m_opModes = new HashMap<>();
  private static final ReentrantLock m_opModesMutex = new ReentrantLock();

  private RobotState() {}

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be enabled.
   *
   * @return True if the robot is enabled, false otherwise.
   */
  public static boolean isEnabled() {
    m_robotStateMutex.lock();
    try {
      return m_controlWord.isEnabled() && m_controlWord.isDSAttached();
    } finally {
      m_robotStateMutex.unlock();
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
    m_robotStateMutex.lock();
    try {
      return m_controlWord.isEStopped();
    } finally {
      m_robotStateMutex.unlock();
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
    m_robotStateMutex.lock();
    try {
      return m_controlWord.getRobotMode();
    } finally {
      m_robotStateMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode.
   *
   * @return True if autonomous mode should be enabled, false otherwise.
   */
  public static boolean isAutonomous() {
    m_robotStateMutex.lock();
    try {
      return m_controlWord.isAutonomous();
    } finally {
      m_robotStateMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode and enabled.
   *
   * @return True if autonomous should be set and the robot should be enabled.
   */
  public static boolean isAutonomousEnabled() {
    m_robotStateMutex.lock();
    try {
      return m_controlWord.isAutonomousEnabled();
    } finally {
      m_robotStateMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controlled mode.
   *
   * @return True if operator-controlled mode should be enabled, false otherwise.
   */
  public static boolean isTeleop() {
    m_robotStateMutex.lock();
    try {
      return m_controlWord.isTeleop();
    } finally {
      m_robotStateMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controller mode and enabled.
   *
   * @return True if operator-controlled mode should be set and the robot should be enabled.
   */
  public static boolean isTeleopEnabled() {
    m_robotStateMutex.lock();
    try {
      return m_controlWord.isTeleopEnabled();
    } finally {
      m_robotStateMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in Utility
   * mode.
   *
   * @return True if utility mode should be enabled, false otherwise.
   */
  public static boolean isUtility() {
    m_robotStateMutex.lock();
    try {
      return m_controlWord.isUtility();
    } finally {
      m_robotStateMutex.unlock();
    }
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in Utility
   * mode and enabled.
   *
   * @return True if utility mode should be set and the robot should be enabled.
   */
  public static boolean isUtilityEnabled() {
    m_robotStateMutex.lock();
    try {
      return m_controlWord.isUtilityEnabled();
    } finally {
      m_robotStateMutex.unlock();
    }
  }

  /**
   * Obtains the opmode name for an opmode ID.
   *
   * @param id The opmode ID.
   * @return The opmode name.
   */
  public static String opModeToString(long id) {
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

    m_robotStateMutex.lock();
    try {
      return m_controlWord.getOpModeId();
    } finally {
      m_robotStateMutex.unlock();
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

    m_robotStateMutex.lock();
    try {
      return m_opMode;
    } finally {
      m_robotStateMutex.unlock();
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
    m_robotStateMutex.lock();
    try {
      return m_controlWord.isDSAttached();
    } finally {
      m_robotStateMutex.unlock();
    }
  }

  /**
   * Gets if the driver station attached to a Field Management System.
   *
   * @return true if the robot is competing on a field being controlled by a Field Management System
   */
  public static boolean isFMSAttached() {
    m_robotStateMutex.lock();
    try {
      return m_controlWord.isFMSAttached();
    } finally {
      m_robotStateMutex.unlock();
    }
  }

  /** Refreshes robot state data. */
  public static void refreshData() {
    DriverStationBackend.refreshControlWordFromCache(m_controlWordCache);
    var opMode = opModeToString(m_controlWordCache.getOpModeId());
    m_robotStateMutex.lock();
    try {
      var controlWord = m_controlWord;
      m_controlWord = m_controlWordCache;
      m_controlWordCache = controlWord;

      m_opMode = opMode;
    } finally {
      m_robotStateMutex.unlock();
    }
  }
}
