package org.wpilib.commands3.button;

import edu.wpi.first.util.ErrorMessages;
import org.wpilib.commands3.Command;

/**
 * A single trigger binding.
 *
 * @param scope The scope in which the binding is active.
 * @param type The type of binding; or, when the bound command should run
 * @param command The bound command. Cannot be null.
 */
record Binding(BindingScope scope, BindingType type, Command command) {
  public Binding {
    ErrorMessages.requireNonNullParam(scope, "scope", "Binding");
    ErrorMessages.requireNonNullParam(type, "type", "Binding");
    ErrorMessages.requireNonNullParam(command, "command", "Binding");
  }
}
