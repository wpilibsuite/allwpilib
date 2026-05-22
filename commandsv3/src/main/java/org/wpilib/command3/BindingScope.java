// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

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
    long currentOpMode = OpModeFetcher.getFetcher().getOpModeId();

    if (currentCommand != null) {
      return new ForCommand(scheduler, currentCommand);
    } else if (currentOpMode != 0) {
      return new ForOpmode(currentOpMode);
    } else {
      return Global.INSTANCE;
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
      return OpModeFetcher.getFetcher().getOpModeId() == opmodeId;
    }
  }
}
