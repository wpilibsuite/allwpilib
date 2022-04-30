// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertThrows;

import org.junit.jupiter.api.Test;

class CommandGroupErrorTest extends CommandTestBase {
  @Test
  void commandInMultipleGroupsTest() {
    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true);
    Command command2 = command2Holder.getMock();

    new ParallelCommandGroup(command1, command2);
    assertThrows(
        IllegalArgumentException.class, () -> new ParallelCommandGroup(command1, command2));
  }

  @Test
  void commandInGroupExternallyScheduledTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();

      new ParallelCommandGroup(command1, command2);

      assertThrows(IllegalArgumentException.class, () -> scheduler.schedule(command1));
    }
  }

  @Test
  void redecoratedCommandErrorTest() {
    Command command = new InstantCommand();

    assertDoesNotThrow(() -> command.withTimeout(10).until(() -> false));
    assertThrows(IllegalArgumentException.class, () -> command.withTimeout(10));
    CommandGroupBase.clearGroupedCommand(command);
    assertDoesNotThrow(() -> command.withTimeout(10));
  }
}
