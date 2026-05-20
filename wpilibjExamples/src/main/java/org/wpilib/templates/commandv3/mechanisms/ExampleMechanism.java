// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.templates.commandv3.mechanisms;

import org.wpilib.command3.Command;
import org.wpilib.command3.Mechanism;
import org.wpilib.command3.Trigger;

/** An example class demonstrating basic use of mechanisms and commands. */
public class ExampleMechanism implements Mechanism {
  public final Trigger exampleCondition = new Trigger(() -> false);

  /**
   * An example command that requires this mechanism. Only one command that requires this mechanism
   * may run at a time - if a command is already running, this command will interrupt it.
   *
   * <p>This example runs indefinitely and prints a message every time it executes.
   *
   * @return An example command.
   */
  public Command exampleCommand() {
    return run(coroutine -> {
          int runCount = 0;
          while (true) {
            runCount++;
            System.out.println("Example command run #" + runCount);
            coroutine.yield();
          }
        })
        .named("Example Command");
  }
}
