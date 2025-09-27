// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import edu.wpi.first.hal.struct.ControlWordStruct;

/** A wrapper for the HALControlWord bitfield. */
public class ControlWord {
  private long m_word;
  private RobotMode m_robotMode = RobotMode.UNKNOWN;
  private boolean m_enabled;
  private boolean m_emergencyStop;
  private boolean m_fmsAttached;
  private boolean m_dsAttached;

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
        (opModeHash & 0x00FFFFFFFFFFFFFFL)
            | ((long) robotMode.getValue() << 56)
            | (enabled ? 0x0400000000000000L : 0)
            | (emergencyStop ? 0x0800000000000000L : 0)
            | (fmsAttached ? 0x1000000000000000L : 0)
            | (dsAttached ? 0x2000000000000000L : 0);
    m_robotMode = robotMode;
    m_enabled = enabled;
    m_emergencyStop = emergencyStop;
    m_fmsAttached = fmsAttached;
    m_dsAttached = dsAttached;
  }

  /**
   * Updates from the native HAL value.
   *
   * @param word value
   */
  public void update(long word) {
    m_word = word;
    m_robotMode = RobotMode.fromInt((int) ((word >> 56) & 3));
    m_enabled = (word & 0x0400000000000000L) != 0;
    m_emergencyStop = (word & 0x0800000000000000L) != 0;
    m_fmsAttached = (word & 0x1000000000000000L) != 0;
    m_dsAttached = (word & 0x2000000000000000L) != 0;
  }

  /**
   * Updates from an existing word.
   *
   * @param word word to update from
   */
  public void update(ControlWord word) {
    m_word = word.m_word;
    m_robotMode = word.m_robotMode;
    m_enabled = word.m_enabled;
    m_emergencyStop = word.m_emergencyStop;
    m_fmsAttached = word.m_fmsAttached;
    m_dsAttached = word.m_dsAttached;
  }

  /**
   * Gets the Enabled flag.
   *
   * @return the Enabled flag
   */
  public boolean isEnabled() {
    return m_enabled;
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
   * Gets the opmode ID.
   *
   * @return the opmode ID
   */
  public long getOpModeId() {
    // if the hash portion is zero, return 0
    if ((m_word & 0x00FFFFFFFFFFFFFFL) == 0) {
      return 0;
    }
    // otherwise return the full ID (which includes the robot mode)
    return m_word & 0x03FFFFFFFFFFFFFFL;
  }

  /**
   * Sets the opmode ID.
   *
   * @param id opmode ID
   */
  public void setOpModeId(long id) {
    m_word &= ~0x03FFFFFFFFFFFFFFL;
    m_word |= id & 0x03FFFFFFFFFFFFFFL;
    // keep robot mode in sync
    m_robotMode = RobotMode.fromInt((int) ((m_word >> 56) & 3));
  }

  /**
   * Gets the E-Stop flag.
   *
   * @return the E-Stop flag
   */
  public boolean isEStopped() {
    return m_emergencyStop;
  }

  /**
   * Gets the FMS attached flag.
   *
   * @return the FMS attached flag
   */
  public boolean isFMSAttached() {
    return m_fmsAttached;
  }

  /**
   * Gets the DS attached flag.
   *
   * @return the DS attached flag
   */
  public boolean isDSAttached() {
    return m_dsAttached;
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
