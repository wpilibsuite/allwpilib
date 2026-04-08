// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrowsExactly;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;

class SchedulerSideloadFunctionTests extends CommandTestBase {
  @Test
  void sideloadThrowingException() {
    m_scheduler.sideload(
        co -> {
          throw new RuntimeException("Bang!");
        });

    // An exception raised in a sideload function should bubble up
    assertEquals(
        "Bang!", assertThrowsExactly(RuntimeException.class, m_scheduler::run).getMessage());
  }

  @Test
  void periodicSideload() {
    AtomicInteger count = new AtomicInteger(0);
    m_scheduler.addPeriodic(count::incrementAndGet);
    assertEquals(0, count.get());

    m_scheduler.run();
    assertEquals(1, count.get());

    m_scheduler.run();
    assertEquals(2, count.get());
  }

  @Test
  void sideloadSchedulingCommand() {
    var command = Command.noRequirements().executing(Coroutine::park).named("Command");
    // one-shot sideload forks a command and immediately exits
    m_scheduler.sideload(co -> co.fork(command));
    m_scheduler.run();
    assertTrue(
        m_scheduler.isRunning(command), "command should have started and outlasted the sideload");
  }

  @Test
  void childCommandEscapesViaSideload() {
    var child = Command.noRequirements().executing(Coroutine::park).named("Child");
    var parent =
        Command.noRequirements()
            .executing(
                parentCoroutine -> {
                  m_scheduler.sideload(sidelodCoroutine -> sidelodCoroutine.fork(child));
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();
    assertFalse(m_scheduler.isScheduledOrRunning(parent), "parent should have exited");
    assertFalse(
        m_scheduler.isScheduledOrRunning(child),
        "the sideload to schedule the child should not have run yet");

    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(child), "child should have started running");
  }

  @Test
  void sideloadCancelingCommand() {
    var command = Command.noRequirements().executing(Coroutine::park).named("Command");
    m_scheduler.schedule(command);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command), "command should have started");

    m_scheduler.sideload(co -> m_scheduler.cancel(command));
    assertTrue(m_scheduler.isRunning(command), "sideload should not have run yet");

    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(command), "sideload should have canceled the command");
  }

  @Test
  void sideloadAffectsStateForTriggerInSameCycle() {
    AtomicBoolean signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements().executing(Coroutine::park).named("Command");
    trigger.onTrue(command);
    m_scheduler.sideload(co -> signal.set(true));

    m_scheduler.run();
    assertTrue(signal.get(), "Sideload should have run and set the signal");
    assertTrue(m_scheduler.isRunning(command), "Command should have started");
  }
}
