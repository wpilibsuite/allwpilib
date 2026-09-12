// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.command3.Scheduler.ScheduleResult.RequiresUnsafeMechanisms;
import org.wpilib.command3.Scheduler.ScheduleResult.Success;
import org.wpilib.command3.SchedulerEvent.Canceled;
import org.wpilib.command3.SchedulerEvent.Interrupted;

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
        Interrupted.class,
        e -> e.command() == parent,
        "Parent should receive an interrupted event");
    assertSchedulerEvent(
        Canceled.class, e -> e.command() == parent, "Parent should receive a cancellation event");
  }

  @Test
  void canScheduleSafeCommandInDisabled() {
    var mech = new DummyMechanism("mech", m_scheduler, true);
    var safeCommand = mech.run(Coroutine::park).named("Safe Command");

    m_enabled = false;

    var result = m_scheduler.schedule(safeCommand);
    assertTrue(result instanceof Success(var cmd) && cmd == safeCommand);

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
}
