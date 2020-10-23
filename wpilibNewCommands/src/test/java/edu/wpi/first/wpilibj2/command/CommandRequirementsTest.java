/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.verify;

class CommandRequirementsTest extends CommandTestBase {
  @Test
  void requirementInterruptTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Subsystem requirement = new TestSubsystem();

      MockCommandHolder interruptedHolder = new MockCommandHolder(true, requirement);
      Command interrupted = interruptedHolder.getMock();
      MockCommandHolder interrupterHolder = new MockCommandHolder(true, requirement);
      Command interrupter = interrupterHolder.getMock();

      scheduler.schedule(interrupted);
      scheduler.run();
      scheduler.schedule(interrupter);
      scheduler.run();

      verify(interrupted).initialize();
      verify(interrupted).execute();
      verify(interrupted).end(true);

      verify(interrupter).initialize();
      verify(interrupter).execute();

      assertFalse(scheduler.isScheduled(interrupted));
      assertTrue(scheduler.isScheduled(interrupter));
    }
  }

  @Test
  void requirementUninterruptibleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Subsystem requirement = new TestSubsystem();

      MockCommandHolder interruptedHolder = new MockCommandHolder(true, requirement);
      Command notInterrupted = interruptedHolder.getMock();
      MockCommandHolder interrupterHolder = new MockCommandHolder(true, requirement);
      Command interrupter = interrupterHolder.getMock();

      scheduler.schedule(false, notInterrupted);
      scheduler.schedule(interrupter);

      assertTrue(scheduler.isScheduled(notInterrupted));
      assertFalse(scheduler.isScheduled(interrupter));
    }
  }

  @Test
  void defaultCommandRequirementErrorTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Subsystem system = new TestSubsystem();

      Command missingRequirement = new WaitUntilCommand(() -> false);
      Command ends = new InstantCommand(() -> {
      }, system);

      assertThrows(IllegalArgumentException.class,
          () -> scheduler.setDefaultCommand(system, missingRequirement));
      assertThrows(IllegalArgumentException.class,
          () -> scheduler.setDefaultCommand(system, ends));
    }
  }
}
