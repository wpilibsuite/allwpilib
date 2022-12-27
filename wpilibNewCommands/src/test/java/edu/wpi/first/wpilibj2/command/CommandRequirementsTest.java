// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.verify;

import org.junit.jupiter.api.Test;

class CommandRequirementsTest extends CommandTestBase {
  @Test
  void requirementInterruptTest() {
    Subsystem requirement = new SubsystemBase() {};

    MockCommandHolder interruptedHolder = new MockCommandHolder(true, requirement);
    Command interrupted = interruptedHolder.getMock();
    MockCommandHolder interrupterHolder = new MockCommandHolder(true, requirement);
    Command interrupter = interrupterHolder.getMock();

    interrupted.schedule();
    CommandScheduler.getInstance().run();
    interrupter.schedule();
    CommandScheduler.getInstance().run();

    verify(interrupted).initialize();
    verify(interrupted).execute();
    verify(interrupted).end(true);

    verify(interrupter).initialize();
    verify(interrupter).execute();

    assertFalse(interrupted.isScheduled());
    assertTrue(interrupter.isScheduled());
  }

  @Test
  void requirementUninterruptibleTest() {
    Subsystem requirement = new SubsystemBase() {};

    Command notInterrupted =
        new RunCommand(() -> {}, requirement)
            .withInterruptBehavior(Command.InterruptionBehavior.kCancelIncoming);
    MockCommandHolder interrupterHolder = new MockCommandHolder(true, requirement);
    Command interrupter = interrupterHolder.getMock();

    notInterrupted.schedule();
    interrupter.schedule();

    assertTrue(notInterrupted.isScheduled());
    assertFalse(interrupter.isScheduled());
  }

  @Test
  void defaultCommandRequirementErrorTest() {
    Subsystem system = new SubsystemBase() {};

    Command missingRequirement = new WaitUntilCommand(() -> false);

    assertThrows(
        IllegalArgumentException.class, () -> system.setDefaultCommand(missingRequirement));
  }
}
