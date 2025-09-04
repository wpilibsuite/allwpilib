// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static edu.wpi.first.units.Units.Microseconds;
import static edu.wpi.first.units.Units.Milliseconds;
import static edu.wpi.first.units.Units.Seconds;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj.RobotController;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicReference;
import org.junit.jupiter.api.Test;

@SuppressWarnings("checkstyle:TypeName")
class SchedulerTest_Timing extends CommandTestBase {
  @Test
  void commandAwaitingItself() {
    // This command deadlocks on itself. It's calling yield() in an infinite loop, which is
    // equivalent to calling Coroutine.park(). No deleterious side effects other than stalling
    // the command
    AtomicReference<Command> commandRef = new AtomicReference<>();
    var command =
        Command.noRequirements().executing(co -> co.await(commandRef.get())).named("Self Await");
    commandRef.set(command);

    m_scheduler.schedule(command);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command));
  }

  @Test
  void commandDeadlock() {
    AtomicReference<Command> ref1 = new AtomicReference<>();
    AtomicReference<Command> ref2 = new AtomicReference<>();

    // Deadlock scenario:
    // command1 starts, schedules command2, then waits for command2 to exit
    // command2 starts, waits for command1 to exit
    //
    // Each successive run sees command1 mount, check for command2, then yield.
    // Then sees command2 mount, check for command1, then also yield.
    // This is like two threads spinwaiting for the other to exit.
    //
    // Externally canceling command2 allows command1 to continue
    // Externally canceling command1 cancels both
    var command1 =
        Command.noRequirements().executing(co -> co.await(ref2.get())).named("Command 1");
    var command2 =
        Command.noRequirements().executing(co -> co.await(ref1.get())).named("Command 2");
    ref1.set(command1);
    ref2.set(command2);

    m_scheduler.schedule(command1);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command1));
    assertTrue(m_scheduler.isRunning(command2));

    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command1));
    assertTrue(m_scheduler.isRunning(command2));

    m_scheduler.cancel(command1);
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(command1));
    assertFalse(m_scheduler.isRunning(command2));
  }

  @Test
  void forkedChildRunsOnce() {
    AtomicInteger runCount = new AtomicInteger(0);

    var inner =
        Command.noRequirements()
            .executing(
                co -> {
                  runCount.incrementAndGet();
                  co.yield();

                  runCount.incrementAndGet();
                  co.yield();
                })
            .named("Inner");

    var outer = Command.noRequirements().executing(co -> co.await(inner)).named("Outer");
    m_scheduler.schedule(outer);
    m_scheduler.run();

    assertEquals(1, runCount.get());
  }

  @Test
  void shortWaitWaitsOneLoop() {
    AtomicLong time = new AtomicLong(0);
    RobotController.setTimeSource(time::get);

    AtomicBoolean completedWait = new AtomicBoolean(false);
    var command =
        Command.noRequirements()
            .executing(
                co -> {
                  co.wait(Milliseconds.of(1));
                  completedWait.set(true);
                })
            .named("Short Wait");

    m_scheduler.schedule(command);
    m_scheduler.run();

    // wait 1 full second (much longer than the wait period)
    time.set((long) Seconds.of(1).in(Microseconds));
    m_scheduler.run();
    assertTrue(
        completedWait.get(),
        "Command with a short wait should have completed if its duration has elapsed between runs");
  }

  @Test
  void shortWaitWaitsOneLoopWithFastPeriod() {
    AtomicLong time = new AtomicLong(0);
    RobotController.setTimeSource(time::get);
    AtomicBoolean completedWait = new AtomicBoolean(false);
    var command =
        Command.noRequirements()
            .executing(
                co -> {
                  co.wait(Milliseconds.of(1));
                  completedWait.set(true);
                })
            .named("Short Wait");

    m_scheduler.schedule(command);
    m_scheduler.run();

    // move forward by half the wait period
    time.set((long) Milliseconds.of(0.5).in(Microseconds));
    m_scheduler.run();
    assertFalse(completedWait.get(), "Command should still be waiting for 1 ms to elapse");

    // move forward by the rest of the wait period
    time.set((long) Milliseconds.of(1).in(Microseconds));
    m_scheduler.run();
    assertTrue(
        completedWait.get(),
        "Command with a short wait should have completed if its duration has elapsed between runs");
  }

  @Test
  void awaitingExitsImmediatelyWithoutAOneLoopDelay() {
    AtomicInteger innerRuns = new AtomicInteger(0);
    var inner =
        Command.noRequirements()
            .executing(
                co -> {
                  // executed immediately when forked
                  innerRuns.incrementAndGet();
                  co.yield();

                  // executed again on the next scheduler run, after the forking command runs
                  innerRuns.incrementAndGet();
                })
            .named("Inner");

    var outer = Command.noRequirements().executing(co -> co.await(inner)).named("Outer");
    m_scheduler.schedule(outer);

    // First run: runs outer, forks inner, inner runs to its first yield, outer yields
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(inner));
    assertTrue(m_scheduler.isRunning(outer));
    assertEquals(1, innerRuns.get());

    // Second run: runs inner to completion, runs outer, outer sees inner is complete and exits
    // NOTE: If child commands ran AFTER their parents, then outer would not have exited here and
    //       would take another scheduler run to complete
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(inner));
    assertFalse(m_scheduler.isRunning(outer));
    assertEquals(2, innerRuns.get());
  }
}
