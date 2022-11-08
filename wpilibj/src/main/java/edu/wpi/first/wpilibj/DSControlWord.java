// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.ControlWord;

/** A wrapper around Driver Station control word. */
public class DSControlWord {
  private final ControlWord m_controlWord = new ControlWord();

  /**
   * DSControlWord constructor.
   *
   * <p>Upon construction, the current Driver Station control word is read and stored internally.
   */
  public DSControlWord() {
    refresh();
  }

  /** Update internal Driver Station control word. */
  public void refresh() {
    DriverStation.refreshControlWordFromCache(m_controlWord);
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be enabled.
   *
   * @return True if the robot is enabled, false otherwise.
   */
  public boolean isEnabled() {
    return m_controlWord.getEnabled() && m_controlWord.getDSAttached();
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
   * Gets a value indicating whether the Robot is e-stopped.
   *
   * @return True if the robot is e-stopped, false otherwise.
   */
  public boolean isEStopped() {
    return m_controlWord.getEStop();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode.
   *
   * @return True if autonomous mode should be enabled, false otherwise.
   */
  public boolean isAutonomous() {
    return m_controlWord.getAutonomous();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode and enabled.
   *
   * @return True if autonomous should be set and the robot should be enabled.
   */
  public boolean isAutonomousEnabled() {
    return m_controlWord.getAutonomous()
        && m_controlWord.getEnabled()
        && m_controlWord.getDSAttached();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controlled mode.
   *
   * @return True if operator-controlled mode should be enabled, false otherwise.
   */
  public boolean isTeleop() {
    return !(isAutonomous() || isTest());
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controller mode and enabled.
   *
   * @return True if operator-controlled mode should be set and the robot should be enabled.
   */
  public boolean isTeleopEnabled() {
    return !m_controlWord.getAutonomous()
        && !m_controlWord.getTest()
        && m_controlWord.getEnabled()
        && m_controlWord.getDSAttached();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in test
   * mode.
   *
   * @return True if test mode should be enabled, false otherwise.
   */
  public boolean isTest() {
    return m_controlWord.getTest();
  }

  /**
   * Gets a value indicating whether the Driver Station is attached.
   *
   * @return True if Driver Station is attached, false otherwise.
   */
  public boolean isDSAttached() {
    return m_controlWord.getDSAttached();
  }

  /**
   * Gets if the driver station attached to a Field Management System.
   *
   * @return true if the robot is competing on a field being controlled by a Field Management System
   */
  public boolean isFMSAttached() {
    return m_controlWord.getFMSAttached();
  }
}
