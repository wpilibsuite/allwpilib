// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import edu.wpi.first.hal.struct.ControlWordStruct;
import java.util.Objects;

/** A wrapper for the HALControlWord bitfield. */
public class ControlWord {
  private boolean m_enabled;
  private boolean m_emergencyStop;
  private boolean m_fmsAttached;
  private boolean m_dsAttached;
  private RobotMode m_robotMode = RobotMode.UNKNOWN;

  /** Default constructor. */
  public ControlWord() {}

  /**
   * Updates from state values.
   *
   * @param enabled enabled
   * @param emergencyStop emergency stopped
   * @param fmsAttached FMS attached
   * @param dsAttached DS attached
   * @param robotMode robot mode
   */
  public void update(
      boolean enabled,
      boolean emergencyStop,
      boolean fmsAttached,
      boolean dsAttached,
      RobotMode robotMode) {
    m_enabled = enabled;
    m_emergencyStop = emergencyStop;
    m_fmsAttached = fmsAttached;
    m_dsAttached = dsAttached;
    m_robotMode = robotMode;
  }

  /**
   * Updates from an existing word.
   *
   * @param word word to update from
   */
  public void update(ControlWord word) {
    m_enabled = word.m_enabled;
    m_robotMode = word.m_robotMode;
    m_emergencyStop = word.m_emergencyStop;
    m_fmsAttached = word.m_fmsAttached;
    m_dsAttached = word.m_dsAttached;
  }

  /**
   * Gets the Enabled flag.
   *
   * @return the Enabled flag
   */
  public boolean getEnabled() {
    return m_enabled;
  }

  /**
   * Gets the robot mode.
   *
   * @return the robot mode
   */
  public RobotMode getMode() {
    return m_robotMode;
  }

  /**
   * Gets the E-Stop flag.
   *
   * @return the E-Stop flag
   */
  public boolean getEStop() {
    return m_emergencyStop;
  }

  /**
   * Gets the FMS attached flag.
   *
   * @return the FMS attached flag
   */
  public boolean getFMSAttached() {
    return m_fmsAttached;
  }

  /**
   * Gets the DS attached flag.
   *
   * @return the DS attached flag
   */
  public boolean getDSAttached() {
    return m_dsAttached;
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof ControlWord word
        && m_enabled == word.m_enabled
        && m_emergencyStop == word.m_emergencyStop
        && m_fmsAttached == word.m_fmsAttached
        && m_dsAttached == word.m_dsAttached
        && m_robotMode.equals(word.m_robotMode);
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_enabled, m_emergencyStop, m_dsAttached, m_fmsAttached, m_robotMode);
  }

  public static final ControlWordStruct struct = new ControlWordStruct();
}
