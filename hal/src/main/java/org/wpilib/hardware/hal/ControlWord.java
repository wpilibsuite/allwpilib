// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

import org.wpilib.hardware.hal.struct.ControlWordStruct;

/** A wrapper for the HALControlWord bitfield. */
public class ControlWord {
  private static final long OPMODE_HASH_MASK = 0x00FFFFFFFFFFFFFFL;
  private static final long ROBOT_MODE_MASK = 0x0300000000000000L;
  private static final long ROBOT_MODE_SHIFT = 56;
  private static final long ENABLED_MASK = 0x0400000000000000L;
  private static final long ESTOP_MASK = 0x0800000000000000L;
  private static final long FMS_ATTACHED_MASK = 0x1000000000000000L;
  private static final long DS_ATTACHED_MASK = 0x2000000000000000L;

  private long m_word;
  private RobotMode m_robotMode = RobotMode.UNKNOWN;

  /** Default constructor. */
  public ControlWord() {}

  /**
   * Updates from state values.
   *
   * @param opModeHash opmode hash
   * @param robotMode robot mode
   * @param enabled enabled
   * @param emergencyStop emergency stopped
   * @param fmsAttached FMS attached
   * @param dsAttached DS attached
   */
  public void update(
      long opModeHash,
      RobotMode robotMode,
      boolean enabled,
      boolean emergencyStop,
      boolean fmsAttached,
      boolean dsAttached) {
    m_word =
        (opModeHash & OPMODE_HASH_MASK)
            | ((long) robotMode.getValue() << ROBOT_MODE_SHIFT)
            | (enabled ? ENABLED_MASK : 0)
            | (emergencyStop ? ESTOP_MASK : 0)
            | (fmsAttached ? FMS_ATTACHED_MASK : 0)
            | (dsAttached ? DS_ATTACHED_MASK : 0);
    m_robotMode = robotMode;
  }

  /**
   * Updates from the native HAL value.
   *
   * @param word value
   */
  public void update(long word) {
    m_word = word;
    m_robotMode = RobotMode.fromInt((int) ((word & ROBOT_MODE_MASK) >> ROBOT_MODE_SHIFT));
  }

  /**
   * Updates from an existing word.
   *
   * @param word word to update from
   */
  public void update(ControlWord word) {
    m_word = word.m_word;
    m_robotMode = word.m_robotMode;
  }

  /**
   * Gets the opmode ID.
   *
   * @return the opmode ID
   */
  public long getOpModeId() {
    // if the hash portion is zero, return 0
    if ((m_word & OPMODE_HASH_MASK) == 0) {
      return 0;
    }
    // otherwise return the full ID (which includes the robot mode)
    return m_word & (OPMODE_HASH_MASK | ROBOT_MODE_MASK);
  }

  /**
   * Sets the opmode ID.
   *
   * @param id opmode ID
   */
  public void setOpModeId(long id) {
    m_word &= ~(OPMODE_HASH_MASK | ROBOT_MODE_MASK);
    m_word |= id & (OPMODE_HASH_MASK | ROBOT_MODE_MASK);
    // keep robot mode in sync
    m_robotMode = RobotMode.fromInt((int) ((m_word & ROBOT_MODE_MASK) >> ROBOT_MODE_SHIFT));
  }

  /**
   * Gets the robot mode.
   *
   * @return the robot mode
   */
  public RobotMode getRobotMode() {
    return m_robotMode;
  }

  /**
   * Gets the Enabled flag.
   *
   * @return the Enabled flag
   */
  public boolean isEnabled() {
    return (m_word & ENABLED_MASK) != 0;
  }

  /**
   * Gets the E-Stop flag.
   *
   * @return the E-Stop flag
   */
  public boolean isEStopped() {
    return (m_word & ESTOP_MASK) != 0;
  }

  /**
   * Gets the FMS attached flag.
   *
   * @return the FMS attached flag
   */
  public boolean isFMSAttached() {
    return (m_word & FMS_ATTACHED_MASK) != 0;
  }

  /**
   * Gets the DS attached flag.
   *
   * @return the DS attached flag
   */
  public boolean isDSAttached() {
    return (m_word & DS_ATTACHED_MASK) != 0;
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode.
   *
   * @return True if autonomous mode should be enabled, false otherwise.
   */
  public boolean isAutonomous() {
    return getRobotMode() == RobotMode.AUTONOMOUS;
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode and enabled.
   *
   * @return True if autonomous should be set and the robot should be enabled.
   */
  public boolean isAutonomousEnabled() {
    return isAutonomous() && isEnabled() && isDSAttached();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controlled mode.
   *
   * @return True if operator-controlled mode should be enabled, false otherwise.
   */
  public boolean isTeleop() {
    return getRobotMode() == RobotMode.TELEOPERATED;
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controller mode and enabled.
   *
   * @return True if operator-controlled mode should be set and the robot should be enabled.
   */
  public boolean isTeleopEnabled() {
    return isTeleop() && isEnabled() && isDSAttached();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in test
   * mode.
   *
   * @return True if test mode should be enabled, false otherwise.
   */
  public boolean isTest() {
    return getRobotMode() == RobotMode.TEST;
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in test
   * mode and enabled.
   *
   * @return True if test mode should be set and the robot should be enabled.
   */
  public boolean isTestEnabled() {
    return isTest() && isEnabled() && isDSAttached();
  }

  /**
   * Gets the native HAL control word value.
   *
   * @return control word value
   */
  public long getNative() {
    return m_word;
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof ControlWord word && m_word == word.m_word;
  }

  @Override
  public int hashCode() {
    return Long.hashCode(m_word);
  }

  public static final ControlWordStruct struct = new ControlWordStruct();
}
