/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import java.util.Map;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;

class SelectCommandTest extends CommandTestBase {
  @Test
  void selectCommandTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      command1Holder.setFinished(true);
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();
      MockCommandHolder command3Holder = new MockCommandHolder(true);
      Command command3 = command3Holder.getMock();

      SelectCommand selectCommand =
          new SelectCommand(Map.ofEntries(
              Map.entry("one", command1),
              Map.entry("two", command2),
              Map.entry("three", command3)),
              () -> "one");

      scheduler.schedule(selectCommand);
      scheduler.run();

      verify(command1).initialize();
      verify(command1).execute();
      verify(command1).end(false);

      verify(command2, never()).initialize();
      verify(command2, never()).execute();
      verify(command2, never()).end(false);

      verify(command3, never()).initialize();
      verify(command3, never()).execute();
      verify(command3, never()).end(false);
    }
  }

  @Test
  void selectCommandInvalidKeyTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      command1Holder.setFinished(true);
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();
      MockCommandHolder command3Holder = new MockCommandHolder(true);
      Command command3 = command3Holder.getMock();

      SelectCommand selectCommand =
          new SelectCommand(Map.ofEntries(
              Map.entry("one", command1),
              Map.entry("two", command2),
              Map.entry("three", command3)),
              () -> "four");

      assertDoesNotThrow(() -> scheduler.schedule(selectCommand));
    }
  }


  @Test
  void selectCommandRequirementTest() {
    Subsystem system1 = new TestSubsystem();
    Subsystem system2 = new TestSubsystem();
    Subsystem system3 = new TestSubsystem();
    Subsystem system4 = new TestSubsystem();

    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true, system3);
      Command command2 = command2Holder.getMock();
      MockCommandHolder command3Holder = new MockCommandHolder(true, system3, system4);
      Command command3 = command3Holder.getMock();

      SelectCommand selectCommand = new SelectCommand(
          Map.ofEntries(Map.entry("one", command1), Map.entry("two", command2),
              Map.entry("three", command3)), () -> "one");

      scheduler.schedule(selectCommand);
      scheduler.schedule(new InstantCommand(() -> {
      }, system3));

      assertFalse(scheduler.isScheduled(selectCommand));

      verify(command1).end(true);
      verify(command2, never()).end(true);
      verify(command3, never()).end(true);
    }
  }
}
