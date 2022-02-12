// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;

class CommandDecoratorTest extends CommandTestBase {
  @Test
  @ResourceLock("timing")
  void withTimeoutTest() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Command command1 = new WaitCommand(10);

      Command timeout = command1.withTimeout(2);

      scheduler.schedule(timeout);
      scheduler.run();

      assertFalse(scheduler.isScheduled(command1));
      assertTrue(scheduler.isScheduled(timeout));

      SimHooks.stepTiming(3);
      scheduler.run();

      assertFalse(scheduler.isScheduled(timeout));
    } finally {
      SimHooks.resumeTiming();
    }
  }

  @Test
  void untilTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      ConditionHolder condition = new ConditionHolder();

      Command command = new WaitCommand(10).until(condition::getCondition);

      scheduler.schedule(command);
      scheduler.run();
      assertTrue(scheduler.isScheduled(command));
      condition.setCondition(true);
      scheduler.run();
      assertFalse(scheduler.isScheduled(command));
    }
  }

  @Test
  void beforeStartingTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      ConditionHolder condition = new ConditionHolder();
      condition.setCondition(false);

      Command command = new InstantCommand();

      scheduler.schedule(command.beforeStarting(() -> condition.setCondition(true)));

      assertTrue(condition.getCondition());
    }
  }

  @Test
  void andThenLambdaTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      ConditionHolder condition = new ConditionHolder();
      condition.setCondition(false);

      Command command = new InstantCommand();

      scheduler.schedule(command.andThen(() -> condition.setCondition(true)));

      assertFalse(condition.getCondition());

      scheduler.run();

      assertTrue(condition.getCondition());
    }
  }

  @Test
  void andThenTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      ConditionHolder condition = new ConditionHolder();
      condition.setCondition(false);

      Command command1 = new InstantCommand();
      Command command2 = new InstantCommand(() -> condition.setCondition(true));

      scheduler.schedule(command1.andThen(command2));

      assertFalse(condition.getCondition());

      scheduler.run();

      assertTrue(condition.getCondition());
    }
  }

  @Test
  void deadlineWithTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      ConditionHolder condition = new ConditionHolder();
      condition.setCondition(false);

      Command dictator = new WaitUntilCommand(condition::getCondition);
      Command endsBefore = new InstantCommand();
      Command endsAfter = new WaitUntilCommand(() -> false);

      Command group = dictator.deadlineWith(endsBefore, endsAfter);

      scheduler.schedule(group);
      scheduler.run();

      assertTrue(scheduler.isScheduled(group));

      condition.setCondition(true);
      scheduler.run();

      assertFalse(scheduler.isScheduled(group));
    }
  }

  @Test
  void alongWithTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      ConditionHolder condition = new ConditionHolder();
      condition.setCondition(false);

      Command command1 = new WaitUntilCommand(condition::getCondition);
      Command command2 = new InstantCommand();

      Command group = command1.alongWith(command2);

      scheduler.schedule(group);
      scheduler.run();

      assertTrue(scheduler.isScheduled(group));

      condition.setCondition(true);
      scheduler.run();

      assertFalse(scheduler.isScheduled(group));
    }
  }

  @Test
  void raceWithTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Command command1 = new WaitUntilCommand(() -> false);
      Command command2 = new InstantCommand();

      Command group = command1.raceWith(command2);

      scheduler.schedule(group);
      scheduler.run();

      assertFalse(scheduler.isScheduled(group));
    }
  }

  @Test
  void perpetuallyTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Command command = new InstantCommand();

      Command perpetual = command.perpetually();

      scheduler.schedule(perpetual);
      scheduler.run();
      scheduler.run();
      scheduler.run();

      assertTrue(scheduler.isScheduled(perpetual));
    }
  }

  @Test
  void onInitTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Counter counter = new Counter();
      Command command = new InstantCommand();

      Command init = command.onInit(counter::increment);

      scheduler.schedule(init);
      scheduler.run();
      assertTrue(counter.m_counter == 1);

      scheduler.schedule(init);
      scheduler.run();
      assertTrue(counter.m_counter == 2);
    }
  }

  @Test
  void onInitRequirementsTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Subsystem instantReq = new TestSubsystem();
      Subsystem initReq = new TestSubsystem();

      Command command = new InstantCommand(()->{}, instantReq);
      Command init = command.onInit(()->{}, initReq);

      assertTrue(init.hasRequirement(instantReq));
      assertTrue(init.hasRequirement(initReq));
    }
  }
}
