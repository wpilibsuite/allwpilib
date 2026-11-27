// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.command3.button.TunableTriggers;
import org.wpilib.event.EventLoop;
import org.wpilib.tunable.MockTunableBackend;
import org.wpilib.tunable.TunableDouble;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.tunable.Tunables;

class TunableTriggersTest extends CommandTestBase {
  private MockTunableBackend m_backend;

  @BeforeEach
  void setUpTunables() {
    TunableRegistry.reset();
    m_backend = new MockTunableBackend();
    TunableRegistry.registerBackend("", m_backend);
  }

  @AfterEach
  void tearDownTunables() {
    TunableRegistry.reset();
  }

  @Test
  void initialFalseAndTuneBeforeFirstPollDetected() {
    EventLoop loop = new EventLoop();
    TunableDouble gain = Tunables.addDouble("gain", 1.0);

    tune("/gain", 2.0);
    Trigger trigger = TunableTriggers.tuned(m_scheduler, loop, gain);

    assertFalse(trigger.getAsBoolean());

    tune("/gain", 3.0);
    assertFalse(trigger.getAsBoolean());

    loop.poll();
    assertTrue(trigger.getAsBoolean());

    loop.poll();
    assertFalse(trigger.getAsBoolean());
  }

  @Test
  void consecutiveAndCoalescedTunesAreReportedPerPoll() {
    EventLoop loop = new EventLoop();
    TunableDouble gain = Tunables.addDouble("gain", 1.0);
    Trigger trigger = TunableTriggers.tuned(m_scheduler, loop, gain);

    loop.poll();
    assertFalse(trigger.getAsBoolean());

    m_backend.setDouble("/gain", 2.0);
    m_backend.setDouble("/gain", 3.0);
    TunableRegistry.update();
    loop.poll();
    assertTrue(trigger.getAsBoolean());

    tune("/gain", 4.0);
    loop.poll();
    assertTrue(trigger.getAsBoolean());

    loop.poll();
    assertFalse(trigger.getAsBoolean());
  }

  @Test
  void localWritesDoNotActivateTrigger() {
    EventLoop loop = new EventLoop();
    TunableDouble gain = Tunables.addDouble("gain", 1.0);
    Trigger trigger = TunableTriggers.tuned(m_scheduler, loop, gain);

    gain.set(2.0);
    loop.poll();

    assertFalse(trigger.getAsBoolean());
  }

  @Test
  void multipleBindingsRepeatedReadsAndIndependentObserversSeeCachedPollState() {
    EventLoop loop = new EventLoop();
    TunableDouble gain = Tunables.addDouble("gain", 1.0);
    Trigger first = TunableTriggers.tuned(m_scheduler, loop, gain);
    Trigger second = TunableTriggers.tuned(m_scheduler, loop, gain);
    AtomicLong firstCount = new AtomicLong();
    AtomicLong secondCount = new AtomicLong();
    Command firstCommand =
        Command.noRequirements(_ -> firstCount.incrementAndGet()).named("First Command");
    Command secondCommand =
        Command.noRequirements(_ -> secondCount.incrementAndGet()).named("Second Command");

    first.ifTrue(firstCommand);
    first.ifTrue(secondCommand);

    tune("/gain", 2.0);
    loop.poll();

    assertTrue(first.getAsBoolean());
    assertTrue(first.getAsBoolean());
    assertTrue(second.getAsBoolean());
    assertTrue(m_scheduler.isScheduled(firstCommand));
    assertTrue(m_scheduler.isScheduled(secondCommand));

    m_scheduler.run();

    assertEquals(1, firstCount.get());
    assertEquals(1, secondCount.get());

    loop.poll();

    assertFalse(first.getAsBoolean());
    assertFalse(second.getAsBoolean());
  }

  @Test
  void loopsPolledAtDifferentRatesMaintainIndependentHistory() {
    EventLoop fastLoop = new EventLoop();
    EventLoop slowLoop = new EventLoop();
    TunableDouble gain = Tunables.addDouble("gain", 1.0);
    Trigger fast = TunableTriggers.tuned(m_scheduler, fastLoop, gain);
    Trigger slow = TunableTriggers.tuned(m_scheduler, slowLoop, gain);
    AtomicBoolean gate = new AtomicBoolean(false);
    Trigger gatedFast = fast.and(gate::get);

    tune("/gain", 2.0);

    fastLoop.poll();
    assertTrue(fast.getAsBoolean());
    assertFalse(gatedFast.getAsBoolean());
    assertFalse(slow.getAsBoolean());

    gate.set(true);
    fastLoop.poll();
    assertFalse(fast.getAsBoolean());
    assertFalse(gatedFast.getAsBoolean());

    slowLoop.poll();
    assertTrue(slow.getAsBoolean());

    slowLoop.poll();
    assertFalse(slow.getAsBoolean());
  }

  @Test
  void orCompositionReportsTunedInputs() {
    EventLoop loop = new EventLoop();
    TunableDouble kP = Tunables.addDouble("kP", 1.0);
    TunableDouble kI = Tunables.addDouble("kI", 0.0);
    TunableDouble kD = Tunables.addDouble("kD", 0.0);
    Trigger trigger =
        TunableTriggers.tuned(m_scheduler, loop, kP)
            .or(TunableTriggers.tuned(m_scheduler, loop, kI))
            .or(TunableTriggers.tuned(m_scheduler, loop, kD));

    loop.poll();
    assertFalse(trigger.getAsBoolean());

    tune("/kI", 1.0);
    loop.poll();
    assertTrue(trigger.getAsBoolean());

    loop.poll();
    assertFalse(trigger.getAsBoolean());

    m_backend.setDouble("/kP", 2.0);
    m_backend.setDouble("/kD", 3.0);
    TunableRegistry.update();
    loop.poll();
    assertTrue(trigger.getAsBoolean());
  }

  @Test
  void selectedSchedulerDefaultLoopAndExplicitLoopAreIsolated() {
    EventLoop explicitLoop = new EventLoop();
    TunableDouble gain = Tunables.addDouble("gain", 1.0);
    Trigger defaultLoopTrigger = TunableTriggers.tuned(m_scheduler, gain);
    Trigger explicitLoopTrigger = TunableTriggers.tuned(m_scheduler, explicitLoop, gain);

    tune("/gain", 2.0);

    explicitLoop.poll();
    assertTrue(explicitLoopTrigger.getAsBoolean());
    assertFalse(defaultLoopTrigger.getAsBoolean());

    m_scheduler.getDefaultEventLoop().poll();
    assertTrue(defaultLoopTrigger.getAsBoolean());
  }

  @Test
  void explicitSchedulerSelectionSchedulesOnSelectedScheduler() {
    Scheduler otherScheduler = Scheduler.createIndependentScheduler();
    TunableDouble gain = Tunables.addDouble("gain", 1.0);
    AtomicLong runs = new AtomicLong();
    Command command = Command.noRequirements(_ -> runs.incrementAndGet()).named("Apply Gains");

    TunableTriggers.tuned(otherScheduler, gain).ifTrue(command);

    tune("/gain", 2.0);
    m_scheduler.run();
    assertEquals(0, runs.get());

    otherScheduler.run();
    assertEquals(1, runs.get());
  }

  @Test
  void ifTrueRunsOnConsecutiveTunePolls() {
    TunableDouble gain = Tunables.addDouble("gain", 1.0);
    AtomicLong runs = new AtomicLong();
    Command applyGains = Command.noRequirements(_ -> runs.incrementAndGet()).named("Apply Gains");

    TunableTriggers.tuned(m_scheduler, gain).ifTrue(applyGains);

    m_scheduler.run();
    assertEquals(0, runs.get());

    tune("/gain", 2.0);
    m_scheduler.run();
    assertEquals(1, runs.get());

    tune("/gain", 3.0);
    m_scheduler.run();
    assertEquals(2, runs.get());

    m_scheduler.run();
    assertEquals(2, runs.get());
  }

  @Test
  void opmodeScopedIfTrueBindingCancelsAndDoesNotReplayAfterScopeEnds() {
    m_opModeId = 1;
    TunableDouble gain = Tunables.addDouble("gain", 1.0);
    AtomicLong cancels = new AtomicLong();
    Command command =
        Command.noRequirements(Coroutine::park).whenCanceled(cancels::incrementAndGet).named("Run");

    TunableTriggers.tuned(m_scheduler, gain).ifTrue(command);

    tune("/gain", 2.0);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command));

    m_opModeId = 0;
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(command));
    assertEquals(1, cancels.get());

    tune("/gain", 3.0);
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(command));
    assertEquals(1, cancels.get());
  }

  @Test
  void commandScopedIfTrueBindingCancelsWithParentAndDoesNotLeaveTriggerBehind() {
    TunableDouble gain = Tunables.addDouble("gain", 1.0);
    AtomicLong childRuns = new AtomicLong();
    AtomicLong childCancels = new AtomicLong();
    Command child =
        Command.noRequirements(
                co -> {
                  childRuns.incrementAndGet();
                  co.park();
                })
            .whenCanceled(childCancels::incrementAndGet)
            .named("Child");
    Command parent =
        Command.noRequirements(
                co -> {
                  TunableTriggers.tuned(m_scheduler, gain).ifTrue(child);
                  co.park();
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();

    tune("/gain", 2.0);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(child));
    assertEquals(1, childRuns.get());

    m_scheduler.cancel(parent);
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(child));
    assertEquals(1, childCancels.get());

    tune("/gain", 3.0);
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(child));
    assertEquals(1, childRuns.get());
  }

  private void tune(String path, double value) {
    m_backend.setDouble(path, value);
    TunableRegistry.update();
  }
}
