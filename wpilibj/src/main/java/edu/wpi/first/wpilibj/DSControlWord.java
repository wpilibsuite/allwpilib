// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.ControlWord;
import edu.wpi.first.hal.RobotMode;

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
  public final void refresh() {
    DriverStation.refreshControlWordFromCache(m_controlWord);
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be enabled.
   *
   * @return True if the robot is enabled, false otherwise.
   */
  public boolean isEnabled() {
    return m_controlWord.isEnabled() && m_controlWord.isDSAttached();
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
   * Gets the opmode ID.
   *
   * @return the opmode ID
   */
  public long getOpModeId() {
    return m_controlWord.getOpModeId();
  }

  /**
   * Gets a value indicating whether the Robot is e-stopped.
   *
   * @return True if the robot is e-stopped, false otherwise.
   */
  public boolean isEStopped() {
    return m_controlWord.isEStopped();
  }

  /**
   * Gets the current robot mode.
   *
   * <p>Note that this does not indicate whether the robot is enabled or disabled.
   *
   * @return robot mode
   */
  public RobotMode getRobotMode() {
    return m_controlWord.getRobotMode();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode.
   *
   * @return True if autonomous mode should be enabled, false otherwise.
   */
  public boolean isAutonomous() {
    return m_controlWord.getRobotMode() == RobotMode.AUTONOMOUS;
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode and enabled.
   *
   * @return True if autonomous should be set and the robot should be enabled.
   */
  public boolean isAutonomousEnabled() {
    return m_controlWord.getRobotMode() == RobotMode.AUTONOMOUS
        && m_controlWord.isEnabled()
        && m_controlWord.isDSAttached();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controlled mode.
   *
   * @return True if operator-controlled mode should be enabled, false otherwise.
   */
  public boolean isTeleop() {
    return m_controlWord.getRobotMode() == RobotMode.TELEOPERATED;
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controller mode and enabled.
   *
   * @return True if operator-controlled mode should be set and the robot should be enabled.
   */
  public boolean isTeleopEnabled() {
    return m_controlWord.getRobotMode() == RobotMode.TELEOPERATED
        && m_controlWord.isEnabled()
        && m_controlWord.isDSAttached();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in test
   * mode.
   *
   * @return True if test mode should be enabled, false otherwise.
   */
  public boolean isTest() {
    return m_controlWord.getRobotMode() == RobotMode.TEST;
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in test
   * mode and enabled.
   *
   * @return True if test mode should be set and the robot should be enabled.
   */
  public boolean isTestEnabled() {
    return m_controlWord.getRobotMode() == RobotMode.TEST
        && m_controlWord.isEnabled()
        && m_controlWord.isDSAttached();
  }

  /**
   * Gets a value indicating whether the Driver Station is attached.
   *
   * @return True if Driver Station is attached, false otherwise.
   */
  public boolean isDSAttached() {
    return m_controlWord.isDSAttached();
  }

  /**
   * Gets if the driver station attached to a Field Management System.
   *
   * @return true if the robot is competing on a field being controlled by a Field Management System
   */
  public boolean isFMSAttached() {
    return m_controlWord.isFMSAttached();
  }

  /**
   * Gets a copy of the underlying control word.
   *
   * @param word destination for control word
   */
  public void getControlWord(ControlWord word) {
    word.update(m_controlWord);
  }
}
