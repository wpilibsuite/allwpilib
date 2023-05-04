// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.verify;

import org.junit.jupiter.api.Test;

class DefaultCommandTest extends CommandTestBase {
  @Test
  void defaultCommandScheduleTest() {
    Subsystem hasDefaultCommand = new SubsystemBase() {};

    MockCommandHolder defaultHolder = new MockCommandHolder(true, hasDefaultCommand);
    Command defaultCommand = defaultHolder.getMock();

    hasDefaultCommand.setDefaultCommand(defaultCommand);
    CommandScheduler.getInstance().run();

    assertTrue(defaultCommand.isScheduled());
  }

  @Test
  void defaultCommandInterruptResumeTest() {
    Subsystem hasDefaultCommand = new SubsystemBase() {};

    MockCommandHolder defaultHolder = new MockCommandHolder(true, hasDefaultCommand);
    Command defaultCommand = defaultHolder.getMock();
    MockCommandHolder interrupterHolder = new MockCommandHolder(true, hasDefaultCommand);
    Command interrupter = interrupterHolder.getMock();

    hasDefaultCommand.setDefaultCommand(defaultCommand);
    CommandScheduler.getInstance().run();
    interrupter.schedule();

    assertFalse(defaultCommand.isScheduled());
    assertTrue(interrupter.isScheduled());

    interrupter.cancel();
    CommandScheduler.getInstance().run();

    assertTrue(defaultCommand.isScheduled());
    assertFalse(interrupter.isScheduled());
  }

  @Test
  void defaultCommandDisableResumeTest() {
    Subsystem hasDefaultCommand = new SubsystemBase() {};

    MockCommandHolder defaultHolder = new MockCommandHolder(false, hasDefaultCommand);
    Command defaultCommand = defaultHolder.getMock();

    hasDefaultCommand.setDefaultCommand(defaultCommand);
    CommandScheduler.getInstance().run();

    assertTrue(defaultCommand.isScheduled());

    setDSEnabled(false);
    CommandScheduler.getInstance().run();

    assertFalse(defaultCommand.isScheduled());

    setDSEnabled(true);
    CommandScheduler.getInstance().run();

    assertTrue(defaultCommand.isScheduled());

    verify(defaultCommand).end(true);
  }
}
