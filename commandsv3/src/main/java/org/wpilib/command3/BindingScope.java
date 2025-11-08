// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

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

  static BindingScope global() {
    return Global.INSTANCE;
  }

  static BindingScope forCommand(Scheduler scheduler, Command command) {
    return new ForCommand(scheduler, command);
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
}
