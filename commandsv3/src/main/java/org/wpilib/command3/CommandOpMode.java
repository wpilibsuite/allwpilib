// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import org.wpilib.driverstation.RobotState;
import org.wpilib.opmode.OpMode;

/**
 * Command-aware opmode descriptor used by {@link CommandRobot}.
 *
 * <p>A {@code CommandOpMode} exposes triggers for selection and active/enabled state, and can
 * produce an {@link OpMode} implementation that runs optional hooks around the default {@link
 * Scheduler} execution.
 */
public final class CommandOpMode {
  private final Trigger m_selected;
  private final Trigger m_active;
  private Runnable m_beforeScheduler;
  private Runnable m_afterScheduler;

  /**
   * Constructs a command opmode descriptor.
   *
   * @param name OpMode name used for Driver Station selection matching.
   * @param beforeScheduler Callback run before scheduler execution each periodic loop.
   * @param afterScheduler Callback run after scheduler execution each periodic loop.
   */
  public CommandOpMode(String name, Runnable beforeScheduler, Runnable afterScheduler) {
    requireNonNullParam(name, "name", "CommandOpMode");
    this.m_selected = new Trigger(() -> RobotState.getOpMode().equals(name));
    this.m_active = m_selected.and(RobotState::isEnabled);
    this.m_beforeScheduler = beforeScheduler;
    this.m_afterScheduler = afterScheduler;
  }

  /**
   * Constructs a command opmode descriptor with no-op scheduler hooks.
   *
   * @param name OpMode name used for Driver Station selection matching.
   */
  public CommandOpMode(String name) {
    this(name, () -> {}, () -> {});
  }

  /**
   * Trigger that is true when this opmode is currently selected on the Driver Station.
   *
   * @return Selection trigger.
   */
  public Trigger selected() {
    return m_selected;
  }

  /**
   * Trigger that is true when this opmode is selected and the robot is enabled.
   *
   * @return Active/enabled trigger.
   */
  public Trigger active() {
    return m_active;
  }

  /**
   * Creates a trigger requiring this opmode to be active plus the provided trigger condition.
   *
   * @param other Additional trigger condition.
   * @return Combined trigger.
   */
  public Trigger active(Trigger other) {
    return active().and(other);
  }

  /**
   * Sets a callback to run before {@link Scheduler#getDefault()} executes each periodic loop.
   *
   * @param beforeScheduler Callback run before scheduler execution.
   */
  public void beforeScheduler(Runnable beforeScheduler) {
    m_beforeScheduler = requireNonNullParam(beforeScheduler, "beforeScheduler", "CommandOpMode");
  }

  /**
   * Sets a callback to run after {@link Scheduler#getDefault()} executes each periodic loop.
   *
   * @param afterScheduler Callback run after scheduler execution.
   */
  public void afterScheduler(Runnable afterScheduler) {
    m_afterScheduler = requireNonNullParam(afterScheduler, "afterScheduler", "CommandOpMode");
  }

  /**
   * Binds this descriptor into an {@link OpMode} instance consumed by the framework.
   *
   * @return OpMode that runs pre-hook, scheduler, then post-hook every loop.
   */
  public OpMode bind() {
    return new Impl();
  }

  private final class Impl implements OpMode {
    @Override
    public void periodic() {
      m_beforeScheduler.run();
      Scheduler.getDefault().run();
      m_afterScheduler.run();
    }
  }
}
