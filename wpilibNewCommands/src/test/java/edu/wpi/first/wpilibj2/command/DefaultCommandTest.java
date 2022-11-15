// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.verify;

import edu.wpi.first.wpilibj.event.BooleanEvent;
import org.junit.jupiter.api.Test;

class DefaultCommandTest extends CommandTestBase {
  @Test
  void defaultCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Subsystem hasDefaultCommand = new SubsystemBase() {};

      MockCommandHolder defaultHolder = new MockCommandHolder(true, hasDefaultCommand);
      Command defaultCommand = defaultHolder.getMock();

      new BooleanEvent(scheduler.getDefaultButtonLoop(), () -> scheduler.requiring(hasDefaultCommand) == null).ifHigh(()->scheduler.schedule(defaultCommand));
      scheduler.run();

      assertTrue(scheduler.isScheduled(defaultCommand));
    }
  }

  @Test
  void defaultCommandInterruptResumeTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Subsystem hasDefaultCommand = new SubsystemBase() {};

      MockCommandHolder defaultHolder = new MockCommandHolder(true, hasDefaultCommand);
      Command defaultCommand = defaultHolder.getMock();
      MockCommandHolder interrupterHolder = new MockCommandHolder(true, hasDefaultCommand);
      Command interrupter = interrupterHolder.getMock();

      scheduler.requiredTrigger(hasDefaultCommand).onFalse(defaultCommand);
      scheduler.run();
      scheduler.schedule(interrupter);

      assertFalse(scheduler.isScheduled(defaultCommand));
      assertTrue(scheduler.isScheduled(interrupter));
      assertTrue(scheduler.isRequired(hasDefaultCommand));

      scheduler.cancel(interrupter);
      assertFalse(scheduler.isRequired(hasDefaultCommand));
      scheduler.run();

      assertTrue(scheduler.isScheduled(defaultCommand));
      assertFalse(scheduler.isScheduled(interrupter));
    }
  }

  @Test
  void defaultCommandDisableResumeTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Subsystem hasDefaultCommand = new SubsystemBase() {};

      MockCommandHolder defaultHolder = new MockCommandHolder(false, hasDefaultCommand);
      Command defaultCommand = defaultHolder.getMock();

      scheduler.requiredTrigger(hasDefaultCommand).onFalse(defaultCommand);
      scheduler.run();

      assertTrue(scheduler.isScheduled(defaultCommand));

      setDSEnabled(false);
      scheduler.run();

      assertFalse(scheduler.isScheduled(defaultCommand));

      setDSEnabled(true);
      scheduler.run();

      assertTrue(scheduler.isScheduled(defaultCommand));

      verify(defaultCommand).end(true);
    }
  }
}
