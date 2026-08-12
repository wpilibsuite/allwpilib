// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import org.wpilib.hardware.hal.RobotMode;

/**
 * A scope for when a binding is live. Bindings tied to a scope must be deleted when the scope
 * becomes inactive.
 */
@SuppressWarnings("PMD.ImplicitFunctionalInterface")
interface BindingScope {
  /**
   * Checks if the scope is active. Bindings for inactive scopes are removed from the scheduler.
   *
   * @return True if the scope is still active, false if not.
   */
  boolean active();

  /**
   * Creates the narrowest binding scope available based on the current state of the scheduler and
   * selected opmode.
   *
   * @param scheduler The scheduler to create the binding scope for.
   * @return The narrowest binding scope available.
   */
  static BindingScope createNarrowestScope(Scheduler scheduler) {
    Command currentCommand = scheduler.currentCommand();
    RobotStateFetcher fetcher = RobotStateFetcher.getFetcher();
    long currentOpMode = fetcher.getOpModeId();
    RobotMode currentRobotMode = fetcher.getRobotMode();

    if (currentCommand != null) {
      // Commands are the narrowest scope, so prioritize them first.
      return new ForCommand(scheduler, currentCommand);
    } else if (currentOpMode != 0) {
      // Opmodes are more specific than general robot mode bindings.
      return new ForOpmode(currentOpMode);
    } else {
      // Not in a command and not in an opmode. Use a robot mode scope, if applicable,
      // or fall back to the global scope if the robot is disabled or in an unrecognized mode.
      // The switch statement deliberately does not have a default case. We want the compiler to
      // tell us to update the switch statement if the enum changes.
      return switch (currentRobotMode) {
        case AUTONOMOUS -> AutonomousMode.INSTANCE;
        case TELEOPERATED -> TeleopMode.INSTANCE;
        case UTILITY -> UtilityMode.INSTANCE;
        case UNKNOWN -> Global.INSTANCE;
      };
    }
  }

  /** A global binding scope. Bindings in this scope are always active. */
  final class Global implements BindingScope {
    // No reason not to be a singleton.
    public static final Global INSTANCE = new Global();

    @Override
    public boolean active() {
      return true;
    }
  }

  /**
   * A binding scoped to the lifetime of a specific command. This should be used when a binding is
   * created within a command, tying the lifetime of the binding to the declaring command.
   *
   * @param scheduler The scheduler managing the command.
   * @param command The command being scoped to.
   */
  record ForCommand(Scheduler scheduler, Command command) implements BindingScope {
    @Override
    public boolean active() {
      return scheduler.isRunning(command);
    }
  }

  /**
   * A binding scoped to a running opmode.
   *
   * @param opmodeId The ID of the opmode that the binding is scoped to.
   */
  record ForOpmode(long opmodeId) implements BindingScope {
    @Override
    public boolean active() {
      return RobotStateFetcher.getFetcher().getOpModeId() == opmodeId;
    }
  }

  /**
   * A binding scoped to the autonomous robot mode, but not any particular opmode. Comes into play
   * when robot programs are using commands v3 but not opmodes.
   */
  final class AutonomousMode implements BindingScope {
    public static final AutonomousMode INSTANCE = new AutonomousMode();

    @Override
    public boolean active() {
      return RobotStateFetcher.getFetcher().getRobotMode() == RobotMode.AUTONOMOUS;
    }
  }

  /**
   * A binding scoped to the teleop robot mode, but not any particular opmode. Comes into play when
   * robot programs are using commands v3 but not opmodes.
   */
  final class TeleopMode implements BindingScope {
    public static final TeleopMode INSTANCE = new TeleopMode();

    @Override
    public boolean active() {
      return RobotStateFetcher.getFetcher().getRobotMode() == RobotMode.TELEOPERATED;
    }
  }

  /**
   * A binding scoped to the utility robot mode, but not any particular opmode. Comes into play when
   * robot programs are using commands v3 but not opmodes.
   */
  final class UtilityMode implements BindingScope {
    public static final UtilityMode INSTANCE = new UtilityMode();

    @Override
    public boolean active() {
      return RobotStateFetcher.getFetcher().getRobotMode() == RobotMode.UTILITY;
    }
  }

  // There is no scope for the "disabled" mode, since it would interfere with the global scope
}
