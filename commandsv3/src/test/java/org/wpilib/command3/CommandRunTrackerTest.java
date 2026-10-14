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
import org.wpilib.command3.Scheduler.ScheduleResult.Successful;

class CommandRunTrackerTest extends CommandTestBase {
  @Test
  void nullParametersThrow() {
    assertThrows(NullPointerException.class, () -> CommandRunTracker.of(null, List.of()));
    assertThrows(NullPointerException.class, () -> CommandRunTracker.of(m_scheduler, null));
  }

  @Test
  void emptyTracker() {
    var tracker = CommandRunTracker.of(m_scheduler, List.of());
    assertFalse(tracker.areAllRunning());
    assertFalse(tracker.isAnyRunning());
  }

  @Test
  void singleCommand() {
    var cmd = Command.noRequirements(Coroutine::park).named("Single");
    var result = m_scheduler.schedule(cmd);
    m_scheduler.run();

    var tracker = CommandRunTracker.of(m_scheduler, List.of((Successful) result));
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
    Successful[] results = new Successful[n];
    for (int i = 0; i < n; i++) {
      commands[i] = Command.noRequirements(Coroutine::park).named("Cmd[" + i + "]");
      results[i] = (Successful) m_scheduler.schedule(commands[i]);
    }
    m_scheduler.run();

    var tracker = CommandRunTracker.of(m_scheduler, List.of(results));
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
    Successful[] results = new Successful[n];
    for (int i = 0; i < n; i++) {
      commands[i] = Command.noRequirements(Coroutine::park).named("LargeCmd[" + i + "]");
      results[i] = (Successful) m_scheduler.schedule(commands[i]);
    }
    m_scheduler.run();

    var tracker = CommandRunTracker.of(m_scheduler, List.of(results));
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

  @Test
  void trackerWithPreCompletedCommand() {
    var cmd = Command.noRequirements(co -> {}).named("PreCompleted");
    var result = (Successful) m_scheduler.schedule(cmd);
    m_scheduler.run();
    assertEquals(0, m_scheduler.runId(cmd));

    var tracker = CommandRunTracker.of(m_scheduler, List.of(result));
    assertFalse(tracker.areAllRunning());
    assertFalse(tracker.isAnyRunning());
  }

  @Test
  void trackerWithRescheduledCommand() {
    var cmd = Command.noRequirements(Coroutine::park).named("Rescheduled");
    var result1 = (Successful) m_scheduler.schedule(cmd);
    m_scheduler.run();
    assertTrue(result1.runId() > 0);

    // Cancel first run
    m_scheduler.cancel(cmd);
    assertEquals(0, m_scheduler.runId(cmd));

    // Reschedule command with a new run ID
    var result2 = (Successful) m_scheduler.schedule(cmd);
    m_scheduler.run();
    assertTrue(result2.runId() > result1.runId());
    assertEquals(result2.runId(), m_scheduler.runId(cmd));

    // Create tracker using result1 (stale run ID)
    var tracker = CommandRunTracker.of(m_scheduler, List.of(result1));
    assertFalse(tracker.areAllRunning());
    assertFalse(tracker.isAnyRunning());
  }

  @Test
  void trackerWithOneShotCommand() {
    var cmd = Command.noRequirements(co -> {}).named("OneShot");
    var result = (Successful) m_scheduler.schedule(cmd);
    assertTrue(result.runId() > 0);

    // Run scheduler so one-shot completes
    m_scheduler.run();
    assertEquals(0, m_scheduler.runId(cmd));

    var tracker = CommandRunTracker.of(m_scheduler, List.of(result));
    assertFalse(tracker.areAllRunning());
    assertFalse(tracker.isAnyRunning());
  }

  @Test
  void largeTrackerWithMixedStaleAndRescheduledCommands() {
    int n = 80;
    Command[] commands = new Command[n];
    Successful[] initialResults = new Successful[n];

    for (int i = 0; i < n; i++) {
      commands[i] = Command.noRequirements(Coroutine::park).named("Cmd[" + i + "]");
      initialResults[i] = (Successful) m_scheduler.schedule(commands[i]);
    }
    m_scheduler.run();

    // Word 0 (0..63):
    // 0..19: complete (cancel)
    // 20..39: complete and reschedule (new run ID)
    // 40..63: still running on original run ID

    // Word 1 (64..79):
    // 64..69: complete (cancel)
    // 70..74: complete and reschedule (new run ID)
    // 75..79: still running on original run ID

    for (int i = 0; i < 20; i++) {
      m_scheduler.cancel(commands[i]);
    }
    for (int i = 20; i < 40; i++) {
      m_scheduler.cancel(commands[i]);
      m_scheduler.schedule(commands[i]);
    }
    for (int i = 64; i < 70; i++) {
      m_scheduler.cancel(commands[i]);
    }
    for (int i = 70; i < 75; i++) {
      m_scheduler.cancel(commands[i]);
      m_scheduler.schedule(commands[i]);
    }
    m_scheduler.run();

    var tracker = CommandRunTracker.of(m_scheduler, List.of(initialResults));

    // Not all original commands are running
    assertFalse(tracker.areAllRunning());
    // Some original commands (40..63 and 75..79) are still running
    assertTrue(tracker.isAnyRunning());

    // Cancel remaining original commands in word 0
    for (int i = 40; i < 64; i++) {
      m_scheduler.cancel(commands[i]);
    }
    // Word 1 commands (75..79) are still running on original run IDs
    assertTrue(tracker.isAnyRunning());

    // Cancel remaining original commands in word 1
    for (int i = 75; i < 80; i++) {
      m_scheduler.cancel(commands[i]);
    }

    // Now all original run IDs are no longer active, even though rescheduled commands (20..39,
    // 70..74) are active in scheduler
    assertFalse(tracker.isAnyRunning());
    assertFalse(tracker.areAllRunning());
  }
}
