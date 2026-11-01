// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;
import org.junit.jupiter.api.Test;
import org.wpilib.command3.Scheduler.ScheduleResult.RequiresUnsafeMechanisms;
import org.wpilib.command3.Scheduler.ScheduleResult.Success;
import org.wpilib.command3.SchedulerEvent.Canceled;
import org.wpilib.command3.SchedulerEvent.ForkFailure;

@SuppressWarnings("PMD.CompareObjectsWithEquals")
class SchedulerDisabledTests extends CommandTestBase {
  @Test
  void cancelsCommandsWhenRobotEntersDisabled() {
    var mech = new DummyMechanism("mech", m_scheduler);
    var command = mech.run(Coroutine::park).named("Command");

    m_scheduler.schedule(command);
    m_scheduler.run();
    assertEquals(List.of(command), m_scheduler.getRunningCommands());

    m_enabled = false;
    m_scheduler.run();
    assertEquals(List.of(), m_scheduler.getRunningCommands(), "Command was not canceled");
    assertSchedulerEvent(
        Canceled.class, e -> e.command() == command, "Cancellation did not emit an event");
  }

  @Test
  void cancelsCompositionWhenRobotEntersDisabled() {
    var mech = new DummyMechanism("mech", m_scheduler);
    var child = mech.run(Coroutine::park).named("Child");
    var parent = Command.noRequirements(coroutine -> coroutine.await(child)).named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();
    assertEquals(List.of(parent, child), m_scheduler.getRunningCommands());

    m_enabled = false;
    m_scheduler.run();
    assertEquals(List.of(), m_scheduler.getRunningCommands(), "Composition was not canceled");
    assertSchedulerEvent(
        Canceled.class, e -> e.command() == parent, "Parent did not receive an event");
    assertSchedulerEvent(
        Canceled.class, e -> e.command() == child, "Child did not receive an event");
  }

  @Test
  void doesNotCancelSafeCommands() {
    var mech = new DummyMechanism("mech", m_scheduler, true);

    var safeCommand = mech.run(Coroutine::park).named("Safe Command");

    m_scheduler.schedule(safeCommand);
    m_scheduler.run();
    assertEquals(List.of(safeCommand), m_scheduler.getRunningCommands());

    m_enabled = false;
    m_scheduler.run();
    assertEquals(
        List.of(safeCommand),
        m_scheduler.getRunningCommands(),
        "Safe command should still be running");
  }

  @Test
  void cannotScheduleUnsafeCommandInDisabled() {
    var mech = new DummyMechanism("mech", m_scheduler);
    var command = mech.run(Coroutine::park).named("Command");

    m_enabled = false;
    var result = m_scheduler.schedule(command);
    assertTrue(
        result instanceof RequiresUnsafeMechanisms(var cmd, var unsafe)
            && cmd == command
            && unsafe.contains(mech),
        result.toString());
  }

  @Test
  void cancelsCompositionWhenChildCannotBeScheduled() {
    var mech = new DummyMechanism("mech", m_scheduler);

    var child = mech.run(Coroutine::park).named("Command");
    var parent =
        Command.noRequirements(
                coroutine -> {
                  coroutine.waitUntil(() -> !m_enabled);

                  coroutine.await(child);
                })
            .named("Parent Command");

    m_scheduler.schedule(parent);
    m_scheduler.run();
    assertEquals(List.of(parent), m_scheduler.getRunningCommands());

    m_enabled = false;
    m_scheduler.run();
    assertEquals(
        List.of(),
        m_scheduler.getRunningCommands(),
        "Entire composition should have been canceled");
    assertSchedulerEvent(
        ForkFailure.class,
        e ->
            e.command() == parent
                && e.failures().size() == 1
                && e.failures().getFirst() instanceof RequiresUnsafeMechanisms(var cmd, var mechs)
                && cmd == child
                && mechs.equals(Set.of(mech)),
        "Parent should receive a ForkFailure event");
    assertSchedulerEvent(
        Canceled.class, e -> e.command() == parent, "Parent should receive a cancellation event");
  }

  @Test
  void canScheduleSafeCommandInDisabled() {
    var mech = new DummyMechanism("mech", m_scheduler, true);
    var safeCommand = mech.run(Coroutine::park).named("Safe Command");

    m_enabled = false;

    var result = m_scheduler.schedule(safeCommand);
    assertTrue(result instanceof Success(var cmd, var _) && cmd == safeCommand);

    m_scheduler.run();
    assertEquals(List.of(safeCommand), m_scheduler.getRunningCommands());
  }

  @Test
  void unsafeDefaultCommandsCannotRunInDisabled() {
    var mech = new DummyMechanism("mech", m_scheduler);
    var command = mech.run(Coroutine::park).named("Command");

    m_enabled = false;

    mech.setDefaultCommand(command);
    m_scheduler.run();
    assertEquals(
        List.of(), m_scheduler.getRunningCommands(), "The default command should not be running");
    assertEquals(
        command, m_scheduler.getDefaultCommandFor(mech), "The default command should be set");
  }

  @Test
  void robotDisabledBetweenQueueAndRun() {
    var mech = new DummyMechanism("mech", m_scheduler);
    var command = mech.run(Coroutine::park).named("Command");

    var result = m_scheduler.schedule(command);
    assertInstanceOf(Success.class, result, "The command should have been scheduled");

    m_enabled = false;
    m_scheduler.run();
    assertEquals(
        List.of(), m_scheduler.getRunningCommands(), "The command should have been canceled");
    assertSchedulerEvent(
        Canceled.class,
        e -> e.command().equals(command),
        "The command should have received a cancellation event");
  }

  @Test
  void nestedForkFailureInDisabledCancelsRootCommand() {
    m_enabled = false;

    var mech = new DummyMechanism("mech", m_scheduler);
    var unsafeCommand = mech.run(Coroutine::park).named("Unsafe Command");

    var ranAfterFork = new AtomicBoolean(false);

    var child =
        Command.noRequirements(
                coroutine -> {
                  coroutine.await(unsafeCommand);
                })
            .named("Child Wrapper");

    var root =
        Command.noRequirements(
                coroutine -> {
                  coroutine.fork(child);
                  ranAfterFork.set(true);
                  coroutine.yield();
                })
            .named("Root Command");

    m_scheduler.schedule(root);
    m_scheduler.run();

    assertFalse(
        ranAfterFork.get(), "Root command should not continue running after child fork failure");
    assertFalse(m_scheduler.isScheduledOrRunning(root), "Root command should have been canceled");
    assertFalse(m_scheduler.isScheduledOrRunning(child), "Child command should have been canceled");

    assertSchedulerEvent(
        ForkFailure.class, e -> e.command() == child, "Child should receive a ForkFailure event");
    assertSchedulerEvent(
        Canceled.class, e -> e.command() == root, "Root should receive a cancellation event");
    assertSchedulerEvent(
        Canceled.class, e -> e.command() == child, "Child should receive a cancellation event");
  }

  @Test
  void delayedForkFailureInDisabledCancelsRootCommand() {
    m_enabled = false;

    var mech = new DummyMechanism("mech", m_scheduler);
    var unsafeCommand = mech.run(Coroutine::park).named("Unsafe Command");

    var ranAfterFork = new AtomicBoolean(false);
    var childRanOnSecondCycle = new AtomicBoolean(false);

    var child =
        Command.noRequirements(
                coroutine -> {
                  coroutine.yield();
                  childRanOnSecondCycle.set(true);
                  coroutine.await(unsafeCommand);
                })
            .named("Child Wrapper");

    var root =
        Command.noRequirements(
                coroutine -> {
                  coroutine.fork(child);
                  coroutine.yield();
                  ranAfterFork.set(true);
                  coroutine.yield();
                })
            .named("Root Command");

    m_scheduler.schedule(root);
    m_scheduler.run();

    assertTrue(m_scheduler.isRunning(root), "Root should be running after cycle 1");
    assertTrue(m_scheduler.isRunning(child), "Child should be running after cycle 1");
    assertFalse(ranAfterFork.get(), "Root should not have run cycle 2 yet");

    m_scheduler.run();

    assertTrue(childRanOnSecondCycle.get(), "Child should have run on cycle 2");
    assertFalse(
        ranAfterFork.get(),
        "Root command should not continue running after delayed child fork failure");
    assertFalse(m_scheduler.isScheduledOrRunning(root), "Root command should have been canceled");
    assertFalse(m_scheduler.isScheduledOrRunning(child), "Child command should have been canceled");

    assertSchedulerEvent(
        ForkFailure.class, e -> e.command() == child, "Child should receive a ForkFailure event");
    assertSchedulerEvent(
        Canceled.class, e -> e.command() == root, "Root should receive a cancellation event");
    assertSchedulerEvent(
        Canceled.class, e -> e.command() == child, "Child should receive a cancellation event");
  }
}
