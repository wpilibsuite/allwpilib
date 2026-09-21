// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;

class CommandRunTrackerTest extends CommandTestBase {
  @Test
  void nullParametersThrow() {
    assertThrows(NullPointerException.class, () -> CommandRunTracker.of(null, new Command[0]));
    assertThrows(
        NullPointerException.class, () -> CommandRunTracker.of(m_scheduler, (Command[]) null));
    assertThrows(
        NullPointerException.class, () -> CommandRunTracker.of(m_scheduler, (Command) null));
    assertThrows(NullPointerException.class, () -> CommandRunTracker.of(null, List.of()));
    assertThrows(
        NullPointerException.class, () -> CommandRunTracker.of(m_scheduler, (List<Command>) null));
  }

  @Test
  void emptyTracker() {
    var tracker = CommandRunTracker.of(m_scheduler, new Command[0]);
    assertFalse(tracker.areAllRunning());
    assertFalse(tracker.isAnyRunning());
  }

  @Test
  void singleCommand() {
    var cmd = Command.noRequirements(Coroutine::park).named("Single");
    m_scheduler.schedule(cmd);
    m_scheduler.run();

    var tracker = CommandRunTracker.of(m_scheduler, cmd);
    assertTrue(tracker.areAllRunning());
    assertTrue(tracker.isAnyRunning());

    m_scheduler.cancel(cmd);
    assertFalse(tracker.areAllRunning());
    assertFalse(tracker.isAnyRunning());
  }

  @Test
  void smallTrackerN64() {
    int n = 64;
    Command[] commands = new Command[n];
    for (int i = 0; i < n; i++) {
      commands[i] = Command.noRequirements(Coroutine::park).named("Cmd[" + i + "]");
      m_scheduler.schedule(commands[i]);
    }
    m_scheduler.run();

    var tracker = CommandRunTracker.of(m_scheduler, commands);
    assertTrue(tracker.areAllRunning());
    assertTrue(tracker.isAnyRunning());

    // Cancel command at index 63 (highest bit in long)
    m_scheduler.cancel(commands[63]);
    assertFalse(tracker.areAllRunning());
    assertTrue(tracker.isAnyRunning());

    // Cancel command at index 0 (lowest bit in long)
    m_scheduler.cancel(commands[0]);
    assertFalse(tracker.areAllRunning());
    assertTrue(tracker.isAnyRunning());

    // Cancel the rest
    for (int i = 1; i < 63; i++) {
      m_scheduler.cancel(commands[i]);
    }
    assertFalse(tracker.areAllRunning());
    assertFalse(tracker.isAnyRunning());
  }

  @Test
  void largeTrackerN100() {
    int n = 100;
    Command[] commands = new Command[n];
    for (int i = 0; i < n; i++) {
      commands[i] = Command.noRequirements(Coroutine::park).named("LargeCmd[" + i + "]");
      m_scheduler.schedule(commands[i]);
    }
    m_scheduler.run();

    var tracker = CommandRunTracker.of(m_scheduler, commands);
    assertTrue(tracker.areAllRunning());
    assertTrue(tracker.isAnyRunning());

    // Cancel index 64 (bit 0 of word 1)
    m_scheduler.cancel(commands[64]);
    assertFalse(tracker.areAllRunning());
    assertTrue(tracker.isAnyRunning());

    // Cancel index 99 (highest element in word 1)
    m_scheduler.cancel(commands[99]);
    assertFalse(tracker.areAllRunning());
    assertTrue(tracker.isAnyRunning());

    // Cancel word 0 commands
    for (int i = 0; i < 64; i++) {
      m_scheduler.cancel(commands[i]);
    }
    assertFalse(tracker.areAllRunning());
    assertTrue(tracker.isAnyRunning());

    // Cancel remaining word 1 commands
    for (int i = 65; i < 99; i++) {
      m_scheduler.cancel(commands[i]);
    }
    assertFalse(tracker.areAllRunning());
    assertFalse(tracker.isAnyRunning());
  }

  @Test
  void partiallyRunningInitially() {
    var running = Command.noRequirements(Coroutine::park).named("Running");
    var notRunning = Command.noRequirements(Coroutine::park).named("NotRunning");

    m_scheduler.schedule(running);
    m_scheduler.run();

    var tracker = CommandRunTracker.of(m_scheduler, running, notRunning);
    assertFalse(tracker.areAllRunning());
    assertTrue(tracker.isAnyRunning());

    m_scheduler.cancel(running);
    assertFalse(tracker.areAllRunning());
    assertFalse(tracker.isAnyRunning());
  }

  @Test
  void coroutineAwaitAllSmallAndLarge() {
    AtomicInteger completedCount = new AtomicInteger(0);

    // Test with 70 commands (Large tracker)
    int n = 70;
    List<Command> children = new ArrayList<>();
    List<AtomicBoolean> flags = new ArrayList<>();
    for (int i = 0; i < n; i++) {
      var flag = new AtomicBoolean(false);
      flags.add(flag);
      children.add(Command.noRequirements(co -> co.waitUntil(flag::get)).named("Child[" + i + "]"));
    }

    var parent =
        Command.noRequirements(
                co -> {
                  co.awaitAll(children);
                  completedCount.incrementAndGet();
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();

    assertEquals(0, completedCount.get());
    assertTrue(m_scheduler.isRunning(parent));

    // Finish first 64
    for (int i = 0; i < 64; i++) {
      flags.get(i).set(true);
    }
    m_scheduler.run();
    assertEquals(0, completedCount.get());

    // Finish remaining
    for (int i = 64; i < n; i++) {
      flags.get(i).set(true);
    }
    m_scheduler.run();
    assertEquals(1, completedCount.get());
    assertFalse(m_scheduler.isRunning(parent));
  }

  @Test
  void coroutineAwaitAnySmallAndLarge() {
    AtomicInteger finishedIndex = new AtomicInteger(-1);

    // Test with 80 commands (Large tracker)
    int n = 80;
    List<Command> children = new ArrayList<>();
    List<AtomicBoolean> flags = new ArrayList<>();
    for (int i = 0; i < n; i++) {
      final int idx = i;
      var flag = new AtomicBoolean(false);
      flags.add(flag);
      children.add(
          Command.noRequirements(
                  co -> {
                    co.waitUntil(flag::get);
                    finishedIndex.set(idx);
                  })
              .named("AnyChild[" + idx + "]"));
    }

    var parent = Command.noRequirements(co -> co.awaitAny(children)).named("ParentAny");

    m_scheduler.schedule(parent);
    m_scheduler.run();

    assertEquals(-1, finishedIndex.get());
    assertTrue(m_scheduler.isRunning(parent));

    // Complete index 75 (in word 1)
    flags.get(75).set(true);
    m_scheduler.run();

    assertEquals(75, finishedIndex.get());
    assertFalse(m_scheduler.isRunning(parent));
    // Siblings should be canceled
    for (int i = 0; i < n; i++) {
      assertFalse(m_scheduler.isRunning(children.get(i)));
    }
  }

  @Test
  void forkResultAwaitCompletion() {
    AtomicBoolean flag1 = new AtomicBoolean(false);
    AtomicBoolean flag2 = new AtomicBoolean(false);
    AtomicBoolean done = new AtomicBoolean(false);

    var c1 = Command.noRequirements(co -> co.waitUntil(flag1::get)).named("C1");

    var c2 = Command.noRequirements(co -> co.waitUntil(flag2::get)).named("C2");

    var parent =
        Command.noRequirements(
                co -> {
                  var forkResult = co.fork(c1, c2);
                  forkResult.awaitCompletion();
                  done.set(true);
                })
            .named("ParentForkAwait");

    m_scheduler.schedule(parent);
    m_scheduler.run();

    assertFalse(done.get());
    assertTrue(m_scheduler.isRunning(c1));
    assertTrue(m_scheduler.isRunning(c2));

    flag1.set(true);
    m_scheduler.run();
    assertFalse(done.get());

    flag2.set(true);
    m_scheduler.run();
    assertTrue(done.get());
  }
}
