// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import org.junit.jupiter.api.Test;

class SchedulerTest extends CommandTestBase {
  @Test
  void basic() {
    var enabled = new AtomicBoolean(false);
    var ran = new AtomicBoolean(false);
    var command =
        Command.noRequirements()
            .executing(
                coroutine -> {
                  do {
                    coroutine.yield();
                  } while (!enabled.get());
                  ran.set(true);
                })
            .named("Basic Command");

    m_scheduler.schedule(command);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command), "Command should be running after being scheduled");

    enabled.set(true);
    m_scheduler.run();
    if (m_scheduler.isRunning(command)) {
      fail("Command should no longer be running after awaiting its completion");
    }

    assertTrue(ran.get());
  }

  @Test
  @SuppressWarnings("PMD.ImmutableField") // PMD bugs
  void atomicity() {
    var mechanism =
        new DummyMechanism("X", m_scheduler) {
          int m_x = 0;
        };

    // Launch 100 commands that each call `x++` 500 times.
    // If commands run on different threads, the lack of atomic
    // operations or locks will mean the final number will be
    // less than the expected 50,000
    int numCommands = 100;
    int iterations = 500;

    for (int cmdCount = 0; cmdCount < numCommands; cmdCount++) {
      var command =
          Command.noRequirements()
              .executing(
                  coroutine -> {
                    for (int i = 0; i < iterations; i++) {
                      mechanism.m_x++;
                      coroutine.yield();
                    }
                  })
              .named("CountCommand[" + cmdCount + "]");

      m_scheduler.schedule(command);
    }

    for (int i = 0; i < iterations; i++) {
      m_scheduler.run();
    }

    assertEquals(numCommands * iterations, mechanism.m_x);
  }

  @Test
  @SuppressWarnings("PMD.ImmutableField") // PMD bugs
  void runMechanism() {
    var example =
        new DummyMechanism("Counting", m_scheduler) {
          int m_x = 0;
        };

    Command countToTen =
        example
            .run(
                coroutine -> {
                  example.m_x = 0;
                  for (int i = 0; i < 10; i++) {
                    coroutine.yield();
                    example.m_x++;
                  }
                })
            .named("Count To Ten");

    m_scheduler.schedule(countToTen);
    for (int i = 0; i < 10; i++) {
      m_scheduler.run();
    }
    m_scheduler.run();

    assertEquals(10, example.m_x);
  }

  @Test
  void compositionsDoNotNeedRequirements() {
    var m1 = new DummyMechanism("M1", m_scheduler);
    var m2 = new DummyMechanism("m2", m_scheduler);

    // the group has no requirements, but can schedule child commands that do
    var group =
        Command.noRequirements()
            .executing(
                co -> {
                  co.awaitAll(
                      m1.run(Coroutine::park).named("M1 Command"),
                      m2.run(Coroutine::park).named("M2 Command"));
                })
            .named("Composition");

    m_scheduler.schedule(group);
    m_scheduler.run(); // start m1 and m2 commands
    assertEquals(3, m_scheduler.getRunningCommands().size());
  }

  @Test
  void nestedMechanisms() {
    var superstructure =
        new DummyMechanism("Superstructure", m_scheduler) {
          private final Mechanism m_elevator = new DummyMechanism("Elevator", m_scheduler);
          private final Mechanism m_arm = new DummyMechanism("Arm", m_scheduler);

          public Command superCommand() {
            return run(co -> {
                  co.await(m_elevator.run(Coroutine::park).named("Elevator Subcommand"));
                  co.await(m_arm.run(Coroutine::park).named("Arm Subcommand"));
                })
                .named("Super Command");
          }
        };

    m_scheduler.schedule(superstructure.superCommand());
    m_scheduler.run();
    assertEquals(
        List.of(superstructure.m_arm.getDefaultCommand()),
        superstructure.m_arm.getRunningCommands(),
        "Arm should only be running its default command");

    // Scheduling something that requires an in-use inner mechanism cancels the outer command
    m_scheduler.schedule(superstructure.m_elevator.run(Coroutine::park).named("Conflict"));

    m_scheduler.run(); // schedules the default superstructure command
    m_scheduler.run(); // starts running the default superstructure command
    assertEquals(List.of(superstructure.getDefaultCommand()), superstructure.getRunningCommands());
  }
}
