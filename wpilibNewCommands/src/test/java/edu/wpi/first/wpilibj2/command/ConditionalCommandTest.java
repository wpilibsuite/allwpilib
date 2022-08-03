// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;

import org.junit.jupiter.api.Test;

class ConditionalCommandTest extends CommandTestBase {
  @Test
  void conditionalCommandTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      command1Holder.setFinished(true);
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();

      ConditionalCommand conditionalCommand =
          new ConditionalCommand(command1, command2, () -> true);

      scheduler.schedule(conditionalCommand);
      scheduler.run();

      verify(command1).initialize();
      verify(command1).execute();
      verify(command1).end(false);

      verify(command2, never()).initialize();
      verify(command2, never()).execute();
      verify(command2, never()).end(false);
    }
  }

  @Test
  void conditionalCommandRequirementTest() {
    Subsystem system1 = new SubsystemBase() {};
    Subsystem system2 = new SubsystemBase() {};
    Subsystem system3 = new SubsystemBase() {};

    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true, system3);
      Command command2 = command2Holder.getMock();

      ConditionalCommand conditionalCommand =
          new ConditionalCommand(command1, command2, () -> true);

      scheduler.schedule(conditionalCommand);
      scheduler.schedule(new InstantCommand(() -> {}, system3));

      assertFalse(scheduler.isScheduled(conditionalCommand));

      verify(command1).end(true);
      verify(command2, never()).end(true);
    }
  }
}
