// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command2.button;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.command2.Command;
import org.wpilib.command2.CommandScheduler;
import org.wpilib.command2.CommandTestBase;
import org.wpilib.command2.Commands;
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
    Trigger trigger = TunableTriggers.tuned(loop, gain);

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
    Trigger trigger = TunableTriggers.tuned(loop, gain);

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
    Trigger trigger = TunableTriggers.tuned(loop, gain);

    gain.set(2.0);
    loop.poll();

    assertFalse(trigger.getAsBoolean());
  }

  @Test
  void multipleBindingsRepeatedReadsAndIndependentObserversSeeCachedPollState() {
    EventLoop loop = new EventLoop();
    TunableDouble gain = Tunables.addDouble("gain", 1.0);
    Trigger first = TunableTriggers.tuned(loop, gain);
    Trigger second = TunableTriggers.tuned(loop, gain);
    AtomicInteger firstCount = new AtomicInteger();
    AtomicInteger secondCount = new AtomicInteger();

    first.ifTrue(Commands.startEnd(firstCount::incrementAndGet, () -> {}));
    first.ifTrue(Commands.startEnd(secondCount::incrementAndGet, () -> {}));

    tune("/gain", 2.0);
    loop.poll();

    assertTrue(first.getAsBoolean());
    assertTrue(first.getAsBoolean());
    assertTrue(second.getAsBoolean());
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
    Trigger fast = TunableTriggers.tuned(fastLoop, gain);
    Trigger slow = TunableTriggers.tuned(slowLoop, gain);
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
        TunableTriggers.tuned(loop, kP)
            .or(TunableTriggers.tuned(loop, kI))
            .or(TunableTriggers.tuned(loop, kD));

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
  void defaultLoopSelectionAndExplicitLoopIsolation() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    EventLoop explicitLoop = new EventLoop();
    TunableDouble gain = Tunables.addDouble("gain", 1.0);
    Trigger defaultTrigger = TunableTriggers.tuned(gain);
    Trigger explicitTrigger = TunableTriggers.tuned(explicitLoop, gain);

    tune("/gain", 2.0);

    explicitLoop.poll();
    assertTrue(explicitTrigger.getAsBoolean());
    assertFalse(defaultTrigger.getAsBoolean());

    scheduler.getDefaultButtonLoop().poll();
    assertTrue(defaultTrigger.getAsBoolean());
  }

  @Test
  void ifTrueRunsOnConsecutiveTunePolls() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    TunableDouble gain = Tunables.addDouble("gain", 1.0);
    AtomicInteger runs = new AtomicInteger();
    Command applyGains = Commands.runOnce(runs::incrementAndGet);

    TunableTriggers.tuned(gain).ifTrue(applyGains);

    scheduler.run();
    assertEquals(0, runs.get());

    tune("/gain", 2.0);
    scheduler.run();
    assertEquals(1, runs.get());

    tune("/gain", 3.0);
    scheduler.run();
    assertEquals(2, runs.get());

    scheduler.run();
    assertEquals(2, runs.get());
  }

  private void tune(String path, double value) {
    m_backend.setDouble(path, value);
    TunableRegistry.update();
  }
}
