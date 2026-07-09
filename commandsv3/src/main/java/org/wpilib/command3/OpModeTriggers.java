// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import org.wpilib.annotation.NoDiscard;
import org.wpilib.driverstation.RobotState;
import org.wpilib.opmode.OpMode;

/**
 * Command-aware opmode descriptor used by {@link CommandOpModes} and {@link CommandRobot}. Use the
 * {@code create*OpMode()} methods to create command-aware opmode descriptors.
 *
 * <p>A {@code CommandOpMode} exposes triggers for selection and active/enabled state, and can
 * produce an {@link OpMode} implementation that runs optional hooks around the default {@link
 * Scheduler} execution.
 */
@NoDiscard
public final class OpModeTriggers {
  private final String m_name;
  private final Trigger m_loaded;
  private final Trigger m_enabled;
  private final Trigger m_disabled;

  /**
   * Constructs a command opmode descriptor.
   *
   * @param name OpMode name used for Driver Station selection matching.
   */
  OpModeTriggers(String name) {
    requireNonNullParam(name, "name", "CommandOpMode");
    this.m_name = name;
    this.m_loaded = new Trigger(() -> OpModeFetcher.getFetcher().getOpModeName().equals(m_name));
    this.m_enabled = m_loaded.and(RobotState::isEnabled);
    this.m_disabled = m_loaded.and(RobotState::isDisabled);
  }

  /**
   * Trigger that is true when this opmode is currently loaded on the Driver Station.
   *
   * @return Loaded trigger.
   */
  public Trigger loaded() {
    return m_loaded;
  }

  /**
   * Trigger that is true when this opmode is loaded and the robot is enabled.
   *
   * @return Active/enabled trigger.
   */
  public Trigger enabled() {
    return m_enabled;
  }

  /**
   * Creates a trigger requiring this opmode to be loaded and the robot enabled plus the provided
   * trigger condition.
   *
   * @param other Additional trigger condition.
   * @return Combined trigger.
   */
  public Trigger enabled(Trigger other) {
    return enabled().and(other);
  }

  /**
   * Trigger that is true when this opmode is loaded and the robot is disabled.
   *
   * @return Disabled trigger.
   */
  public Trigger disabled() {
    return m_disabled;
  }

  /**
   * Creates a trigger requiring this opmode to be loaded and the robot disabled plus the provided
   * trigger condition.
   *
   * @param other Additional trigger condition.
   * @return Combined trigger.
   */
  public Trigger disabled(Trigger other) {
    return disabled().and(other);
  }

  /**
   * Sets the default command for a mechanism during this opmode.
   *
   * @param mechanism The mechanism
   * @param command The default command
   * @see Scheduler#setDefaultCommand(String, Mechanism, Command)
   */
  public void setDefaultCommand(Mechanism mechanism, Command command) {
    Scheduler.getDefault().setDefaultCommand(m_name, mechanism, command);
  }
}
