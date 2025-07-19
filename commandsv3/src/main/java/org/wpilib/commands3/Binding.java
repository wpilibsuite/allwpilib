// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import edu.wpi.first.util.ErrorMessages;

/**
 * A single trigger binding.
 *
 * @param scope The scope in which the binding is active.
 * @param type The type of binding; or, when the bound command should run
 * @param command The bound command. Cannot be null.
 * @param frames The stack frames when the binding was created. Used for telemetry and error
 *               reporting so if a command throws an exception, we can tell users where that
 *               command was bound instead of giving a fairly useless backtrace of the command
 *               framework.
 */
record Binding(
    BindingScope scope,
    BindingType type,
    Command command,
    StackTraceElement[] frames) {
  public Binding {
    ErrorMessages.requireNonNullParam(scope, "scope", "Binding");
    ErrorMessages.requireNonNullParam(type, "type", "Binding");
    ErrorMessages.requireNonNullParam(command, "command", "Binding");
  }
}
