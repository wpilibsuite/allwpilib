package org.wpilib.commands3.button;

import org.wpilib.commands3.Command;
import org.wpilib.commands3.Scheduler;

/**
 * A scope for when a binding is live. Bindings tied to a scope must be deleted when the scope
 * becomes inactive.
 */
sealed interface BindingScope {
  /**
   * Checks if the scope is active. Bindings for inactive scopes are removed from the scheduler.
   *
   * @return True if the scope is still active, false if not.
   */
  boolean active();

  /**
   * A global binding scope. Bindings in this scope are always active.
   */
  final class Global implements BindingScope {
    // No reason not to be a singleton.
    public static final Global INSTANCE = new Global();

    @Override
    public boolean active() {
      return true;
    }
  }

  /**
   * A binding scoped to the lifetime of a specific command. This should be used when a binding
   * is created within a command, tying the lifetime of the binding to the declaring command.
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
