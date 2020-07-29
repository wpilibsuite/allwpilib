/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertThrows;

class CommandGroupErrorTest extends CommandTestBase {
  @Test
  void commandInMultipleGroupsTest() {

    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true);
    Command command2 = command2Holder.getMock();

    @SuppressWarnings("PMD.UnusedLocalVariable")
    Command group = new ParallelCommandGroup(command1, command2);
    assertThrows(IllegalArgumentException.class,
        () -> new ParallelCommandGroup(command1, command2));
  }

  @Test
  void commandInGroupExternallyScheduledTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();

      @SuppressWarnings("PMD.UnusedLocalVariable")
      Command group = new ParallelCommandGroup(command1, command2);

      assertThrows(IllegalArgumentException.class,
          () -> scheduler.schedule(command1));
    }
  }

  @Test
  void redecoratedCommandErrorTest() {
    Command command = new InstantCommand();

    assertDoesNotThrow(() -> command.withTimeout(10).withInterrupt(() -> false));
    assertThrows(IllegalArgumentException.class, () -> command.withTimeout(10));
    CommandGroupBase.clearGroupedCommand(command);
    assertDoesNotThrow(() -> command.withTimeout(10));
  }
}
