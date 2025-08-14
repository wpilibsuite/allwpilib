// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;
import static org.junit.jupiter.api.Assertions.assertThrowsExactly;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

import edu.wpi.first.wpilibj.RobotController;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Supplier;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;

@Timeout(5)
class SchedulerTest {
  private Scheduler m_scheduler;

  @BeforeEach
  void setup() {
    RobotController.setTimeSource(() -> System.nanoTime() / 1000L);
    m_scheduler = new Scheduler();
  }

  @Test
  void basic() {
    var enabled = new AtomicBoolean(false);
    var ran = new AtomicBoolean(false);
    var command =
        Command.noRequirements(
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
  void higherPriorityCancels() {
    final var subsystem = new RequireableResource("Subsystem", m_scheduler);

    final var lower = new PriorityCommand(-1000, subsystem);
    final var higher = new PriorityCommand(+1000, subsystem);

    m_scheduler.schedule(lower);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(lower));

    m_scheduler.schedule(higher);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(higher));
    assertFalse(m_scheduler.isRunning(lower));
  }

  @Test
  void lowerPriorityDoesNotCancel() {
    final var subsystem = new RequireableResource("Subsystem", m_scheduler);

    final var lower = new PriorityCommand(-1000, subsystem);
    final var higher = new PriorityCommand(+1000, subsystem);

    m_scheduler.schedule(higher);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(higher));

    m_scheduler.schedule(lower);
    m_scheduler.run();
    if (!m_scheduler.isRunning(higher)) {
      fail("Higher priority command should still be running");
    }
    if (m_scheduler.isRunning(lower)) {
      fail("Lower priority command should not be running");
    }
  }

  @Test
  void samePriorityCancels() {
    final var subsystem = new RequireableResource("Subsystem", m_scheduler);

    final var first = new PriorityCommand(512, subsystem);
    final var second = new PriorityCommand(512, subsystem);

    m_scheduler.schedule(first);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(first));

    m_scheduler.schedule(second);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(second), "New command should be running");
    assertFalse(m_scheduler.isRunning(first), "Old command should be canceled");
  }

  @Test
  @SuppressWarnings({"PMD.RedundantFieldInitializer", "PMD.ImmutableField"}) // PMD bugs
  void atomicity() {
    var resource =
        new RequireableResource("X", m_scheduler) {
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
          Command.noRequirements(
                  coroutine -> {
                    for (int i = 0; i < iterations; i++) {
                      coroutine.yield();
                      resource.m_x++;
                    }
                  })
              .named("CountCommand[" + cmdCount + "]");

      m_scheduler.schedule(command);
    }

    for (int i = 0; i < iterations; i++) {
      m_scheduler.run();
    }
    m_scheduler.run();

    assertEquals(numCommands * iterations, resource.m_x);
  }

  @Test
  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  void errorDetection() {
    var resource = new RequireableResource("X", m_scheduler);

    var command =
        resource
            .run(
                coroutine -> {
                  throw new RuntimeException("The exception");
                })
            .named("Bad Behavior");

    new Trigger(m_scheduler, () -> true).onTrue(command);

    try {
      m_scheduler.run();
      fail("An exception should have been thrown");
    } catch (RuntimeException e) {
      assertEquals("The exception", e.getMessage());

      assertEquals("org.wpilib.commands3.SchedulerTest", e.getStackTrace()[0].getClassName());
      assertEquals("lambda$errorDetection$3", e.getStackTrace()[0].getMethodName());

      assertEquals("=== Command Binding Trace ===", e.getStackTrace()[2].getClassName());

      assertEquals(getClass().getName(), e.getStackTrace()[3].getClassName());
      assertEquals("errorDetection", e.getStackTrace()[3].getMethodName());
    } catch (Throwable t) {
      fail("Expected a RuntimeException to be thrown, but got " + t);
    }
  }

  @Test
  void nestedErrorDetection() {
    var command =
        Command.noRequirements(
                co -> {
                  co.await(
                      Command.noRequirements(
                              c2 -> {
                                new Trigger(m_scheduler, () -> true)
                                    .onTrue(
                                        Command.noRequirements(
                                                c3 -> {
                                                  // Throws IndexOutOfBoundsException
                                                  new ArrayList<>(0).get(-1);
                                                })
                                            .named("Throws IndexOutOfBounds"));
                                c2.park();
                              })
                          .named("Schedules With Trigger"));
                })
            .named("Schedules Directly");

    m_scheduler.schedule(command);

    // The first run sets up the trigger, but does not fire
    // The second run will fire the trigger and cause the inner command to run and throw
    m_scheduler.run();

    try {
      m_scheduler.run();
      fail("Index OOB exception expected");
    } catch (IndexOutOfBoundsException e) {
      StackTraceElement[] stackTrace = e.getStackTrace();

      assertEquals("Index -1 out of bounds for length 0", e.getMessage());
      int nestedIndex = 0;
      for (; nestedIndex < stackTrace.length; nestedIndex++) {
        if (stackTrace[nestedIndex].getClassName().equals(getClass().getName())) {
          break;
        }
      }

      // user code trace for the scheduler run invocation (to `scheduler.run()` in the try block)
      assertEquals("lambda$nestedErrorDetection$6", stackTrace[nestedIndex].getMethodName());
      assertEquals("nestedErrorDetection", stackTrace[nestedIndex + 1].getMethodName());

      // user code trace for where the command was scheduled (the `.onTrue()` line)
      assertEquals("=== Command Binding Trace ===", stackTrace[nestedIndex + 2].getClassName());
      assertEquals("lambda$nestedErrorDetection$7", stackTrace[nestedIndex + 3].getMethodName());
      assertEquals("lambda$nestedErrorDetection$8", stackTrace[nestedIndex + 4].getMethodName());
      assertEquals("nestedErrorDetection", stackTrace[nestedIndex + 5].getMethodName());
    } catch (Throwable unexpected) {
      fail("Expected an IndexOutOfBoundsException to have been thrown, but got" + unexpected);
    }
  }

  @Test
  @SuppressWarnings({"PMD.RedundantFieldInitializer", "PMD.ImmutableField"}) // PMD bugs
  void runResource() {
    var example =
        new RequireableResource("Counting", m_scheduler) {
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
  void cancelOnInterruptDoesNotResume() {
    var count = new AtomicInteger(0);

    var resource = new RequireableResource("Resource", m_scheduler);

    var interrupter =
        Command.requiring(resource).executing(coroutine -> {}).withPriority(2).named("Interrupter");

    var canceledCommand =
        Command.requiring(resource)
            .executing(
                coroutine -> {
                  count.set(1);
                  coroutine.yield();
                  count.set(2);
                })
            .withPriority(1)
            .named("Cancel By Default");

    m_scheduler.schedule(canceledCommand);
    m_scheduler.run();

    m_scheduler.schedule(interrupter);
    m_scheduler.run();
    assertEquals(1, count.get()); // the second "set" call should not have run
  }

  @Test
  void scheduleOverDefaultDoesNotRescheduleDefault() {
    var count = new AtomicInteger(0);

    var resource = new RequireableResource("Resource", m_scheduler);
    var defaultCmd =
        resource
            .run(
                coroutine -> {
                  while (true) {
                    count.incrementAndGet();
                    coroutine.yield();
                  }
                })
            .withPriority(-1)
            .named("Default Command");

    final var newerCmd = resource.run(coroutine -> {}).named("Newer Command");
    resource.setDefaultCommand(defaultCmd);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(defaultCmd), "Default command should be running");

    m_scheduler.schedule(newerCmd);
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(defaultCmd), "Default command should have been interrupted");
    assertEquals(1, count.get(), "Default command should have run once");

    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(defaultCmd));
  }

  @Test
  void cancelsEvictsOnDeck() {
    var command = Command.noRequirements(Coroutine::park).named("Command");
    m_scheduler.schedule(command);
    m_scheduler.cancel(command);
    assertFalse(m_scheduler.isScheduledOrRunning(command));
  }

  @Test
  void cancelAlEvictsOnDeck() {
    var command = Command.noRequirements(Coroutine::park).named("Command");
    m_scheduler.schedule(command);
    m_scheduler.cancelAll();
    assertFalse(m_scheduler.isScheduledOrRunning(command));
  }

  @Test
  void cancelAllCancelsAll() {
    var commands = new ArrayList<Command>(10);
    for (int i = 1; i <= 10; i++) {
      commands.add(Command.noRequirements(Coroutine::yield).named("Command " + i));
    }
    commands.forEach(m_scheduler::schedule);
    m_scheduler.run();
    m_scheduler.cancelAll();
    for (Command command : commands) {
      if (m_scheduler.isRunning(command)) {
        fail(command.name() + " was not canceled by cancelAll()");
      }
    }
  }

  @Test
  void cancelAllStartsDefaults() {
    var resources = new ArrayList<RequireableResource>(10);
    for (int i = 1; i <= 10; i++) {
      resources.add(new RequireableResource("System " + i, m_scheduler));
    }

    var command =
        new CommandBuilder().requiring(resources).executing(Coroutine::yield).named("Big Command");

    // Scheduling the command should evict the on-deck default commands
    m_scheduler.schedule(command);

    // Then running should get it into the set of running commands
    m_scheduler.run();

    // Cancelling should clear out the set of running commands
    m_scheduler.cancelAll();

    // Then ticking the scheduler once to fully remove the command and schedule the defaults
    m_scheduler.run();

    // Then one more tick to start running the scheduled defaults
    m_scheduler.run();

    if (m_scheduler.isRunning(command)) {
      System.err.println(m_scheduler.getRunningCommands());
      fail(command.name() + " was not canceled by cancelAll()");
    }

    for (var resource : resources) {
      var runningCommands = m_scheduler.getRunningCommandsFor(resource);
      assertEquals(
          1,
          runningCommands.size(),
          "Resource " + resource + " should have exactly one running command");
      assertInstanceOf(
          IdleCommand.class,
          runningCommands.getFirst(),
          "Resource " + resource + " is not running the default command");
    }
  }

  @Test
  void cancelDeeplyNestedCompositions() {
    Command root =
        Command.noRequirements(
                co -> {
                  co.await(
                      Command.noRequirements(
                              co2 -> {
                                co2.await(
                                    Command.noRequirements(
                                            co3 -> {
                                              co3.await(
                                                  Command.noRequirements(Coroutine::park)
                                                      .named("Park"));
                                            })
                                        .named("C3"));
                              })
                          .named("C2"));
                })
            .named("Root");

    m_scheduler.schedule(root);

    m_scheduler.run();
    assertEquals(4, m_scheduler.getRunningCommands().size());

    m_scheduler.cancel(root);
    assertEquals(0, m_scheduler.getRunningCommands().size());
  }

  @Test
  void compositionsDoNotSelfCancel() {
    var res = new RequireableResource("The Resource", m_scheduler);
    var group =
        res.run(
                co -> {
                  co.await(
                      res.run(
                              co2 -> {
                                co2.await(
                                    res.run(
                                            co3 -> {
                                              co3.await(res.run(Coroutine::park).named("Park"));
                                            })
                                        .named("C3"));
                              })
                          .named("C2"));
                })
            .named("Group");

    m_scheduler.schedule(group);
    m_scheduler.run();
    assertEquals(4, m_scheduler.getRunningCommands().size());
    assertTrue(m_scheduler.isRunning(group));
  }

  @Test
  void compositionsDoNotCancelParent() {
    var res = new RequireableResource("The Resource", m_scheduler);
    var group =
        res.run(
                co -> {
                  co.fork(res.run(Coroutine::park).named("First Child"));
                  co.fork(res.run(Coroutine::park).named("Second Child"));
                  co.park();
                })
            .named("Group");

    m_scheduler.schedule(group);
    m_scheduler.run();

    // second child interrupts first child
    assertEquals(
        List.of("Group", "Second Child"),
        m_scheduler.getRunningCommands().stream().map(Command::name).toList());
  }

  @Test
  void compositionsDoNotNeedRequirements() {
    var r1 = new RequireableResource("R1", m_scheduler);
    var r2 = new RequireableResource("r2", m_scheduler);

    // the group has no requirements, but can schedule child commands that do
    var group =
        Command.noRequirements(
                co -> {
                  co.awaitAll(
                      r1.run(Coroutine::park).named("R1 Command"),
                      r2.run(Coroutine::park).named("R2 Command"));
                })
            .named("Composition");

    m_scheduler.schedule(group);
    m_scheduler.run(); // start r1 and r2 commands
    assertEquals(3, m_scheduler.getRunningCommands().size());
  }

  @Test
  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  void compositionsCannotAwaitConflictingCommands() {
    var res = new RequireableResource("The Resource", m_scheduler);

    var group =
        Command.noRequirements(
                co -> {
                  co.awaitAll(
                      res.run(Coroutine::park).named("First"),
                      res.run(Coroutine::park).named("Second"));
                })
            .named("Group");

    m_scheduler.schedule(group);

    // Running should attempt to schedule multiple conflicting commands
    try {
      m_scheduler.run();
      fail("An exception should have been thrown");
    } catch (IllegalArgumentException iae) {
      assertEquals(
          "Command Second requires resources that are already used by First. "
              + "Both require The Resource",
          iae.getMessage());
    } catch (Exception e) {
      fail("Unexpected exception: " + e);
    }
  }

  @Test
  void doesNotRunOnCancelWhenInterruptingOnDeck() {
    var ran = new AtomicBoolean(false);

    var resource = new RequireableResource("The Resource", m_scheduler);
    var cmd = resource.run(Coroutine::yield).whenCanceled(() -> ran.set(true)).named("cmd");
    var interrupter = resource.run(Coroutine::yield).named("Interrupter");
    m_scheduler.schedule(cmd);
    m_scheduler.schedule(interrupter);
    m_scheduler.run();

    assertFalse(ran.get(), "onCancel ran when it shouldn't have!");
  }

  @Test
  void doesNotRunOnCancelWhenCancellingOnDeck() {
    var ran = new AtomicBoolean(false);

    var resource = new RequireableResource("The Resource", m_scheduler);
    var cmd = resource.run(Coroutine::yield).whenCanceled(() -> ran.set(true)).named("cmd");
    m_scheduler.schedule(cmd);
    // cancelling before calling .run()
    m_scheduler.cancel(cmd);
    m_scheduler.run();

    assertFalse(ran.get(), "onCancel ran when it shouldn't have!");
  }

  @Test
  void runsOnCancelWhenInterruptingCommand() {
    var ran = new AtomicBoolean(false);

    var resource = new RequireableResource("The Resource", m_scheduler);
    var cmd = resource.run(Coroutine::park).whenCanceled(() -> ran.set(true)).named("cmd");
    var interrupter = resource.run(Coroutine::park).named("Interrupter");
    m_scheduler.schedule(cmd);
    m_scheduler.run();
    m_scheduler.schedule(interrupter);
    m_scheduler.run();

    assertTrue(ran.get(), "onCancel should have run!");
  }

  @Test
  void doesNotRunOnCancelWhenCompleting() {
    var ran = new AtomicBoolean(false);

    var resource = new RequireableResource("The Resource", m_scheduler);
    var cmd = resource.run(Coroutine::yield).whenCanceled(() -> ran.set(true)).named("cmd");
    m_scheduler.schedule(cmd);
    m_scheduler.run();
    m_scheduler.run();

    assertFalse(m_scheduler.isScheduledOrRunning(cmd));
    assertFalse(ran.get(), "onCancel ran when it shouldn't have!");
  }

  @Test
  void runsOnCancelWhenCancelling() {
    var ran = new AtomicBoolean(false);

    var resource = new RequireableResource("The Resource", m_scheduler);
    var cmd = resource.run(Coroutine::yield).whenCanceled(() -> ran.set(true)).named("cmd");
    m_scheduler.schedule(cmd);
    m_scheduler.run();
    m_scheduler.cancel(cmd);

    assertTrue(ran.get(), "onCancel should have run!");
  }

  @Test
  void runsOnCancelWhenCancellingParent() {
    var ran = new AtomicBoolean(false);

    var resource = new RequireableResource("The Resource", m_scheduler);
    var cmd = resource.run(Coroutine::yield).whenCanceled(() -> ran.set(true)).named("cmd");

    var group = new Sequence("Seq", Collections.singletonList(cmd));
    m_scheduler.schedule(group);
    m_scheduler.run();
    m_scheduler.cancel(group);

    assertTrue(ran.get(), "onCancel should have run!");
  }

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
  void nestedResources() {
    var superstructure =
        new RequireableResource("Superstructure", m_scheduler) {
          private final RequireableResource m_elevator =
              new RequireableResource("Elevator", m_scheduler);
          private final RequireableResource m_arm = new RequireableResource("Arm", m_scheduler);

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

    // Scheduling something that requires an in-use inner resource cancels the outer command
    m_scheduler.schedule(superstructure.m_elevator.run(Coroutine::park).named("Conflict"));

    m_scheduler.run(); // schedules the default superstructure command
    m_scheduler.run(); // starts running the default superstructure command
    assertEquals(List.of(superstructure.getDefaultCommand()), superstructure.getRunningCommands());
  }

  @Test
  void protobuf() {
    var res = new RequireableResource("The Resource", m_scheduler);
    var parkCommand = res.run(Coroutine::park).named("Park");
    var c3Command = res.run(co -> co.await(parkCommand)).named("C3");
    var c2Command = res.run(co -> co.await(c3Command)).named("C2");
    var group = res.run(co -> co.await(c2Command)).named("Group");

    m_scheduler.schedule(group);
    m_scheduler.run();

    var scheduledCommand1 = Command.noRequirements(Coroutine::park).named("Command 1");
    var scheduledCommand2 = Command.noRequirements(Coroutine::park).named("Command 2");
    m_scheduler.schedule(scheduledCommand1);
    m_scheduler.schedule(scheduledCommand2);

    var message = Scheduler.proto.createMessage();
    Scheduler.proto.pack(message, m_scheduler);
    var messageJson = message.toString();
    assertEquals(
        """
        {
          "lastTimeMs": %s,
          "queuedCommands": [{
            "priority": 0,
            "id": %s,
            "name": "Command 1"
          }, {
            "priority": 0,
            "id": %s,
            "name": "Command 2"
          }],
          "runningCommands": [{
            "lastTimeMs": %s,
            "totalTimeMs": %s,
            "priority": 0,
            "id": %s,
            "name": "Group",
            "requirements": [{
              "name": "The Resource"
            }]
          }, {
            "lastTimeMs": %s,
            "totalTimeMs": %s,
            "priority": 0,
            "id": %s,
            "parentId": %s,
            "name": "C2",
            "requirements": [{
              "name": "The Resource"
            }]
          }, {
            "lastTimeMs": %s,
            "totalTimeMs": %s,
            "priority": 0,
            "id": %s,
            "parentId": %s,
            "name": "C3",
            "requirements": [{
              "name": "The Resource"
            }]
          }, {
            "lastTimeMs": %s,
            "totalTimeMs": %s,
            "priority": 0,
            "id": %s,
            "parentId": %s,
            "name": "Park",
            "requirements": [{
              "name": "The Resource"
            }]
          }]
        }"""
            .formatted(
                // Scheduler data
                m_scheduler.lastRuntimeMs(),

                // On deck commands
                m_scheduler.runId(scheduledCommand1),
                m_scheduler.runId(scheduledCommand2),

                // Running commands
                m_scheduler.lastCommandRuntimeMs(group),
                m_scheduler.totalRuntimeMs(group),
                m_scheduler.runId(group), // id
                // top-level command, no parent ID

                m_scheduler.lastCommandRuntimeMs(c2Command),
                m_scheduler.totalRuntimeMs(c2Command),
                m_scheduler.runId(c2Command), // id
                m_scheduler.runId(group), // parent
                m_scheduler.lastCommandRuntimeMs(c3Command),
                m_scheduler.totalRuntimeMs(c3Command),
                m_scheduler.runId(c3Command), // id
                m_scheduler.runId(c2Command), // parent
                m_scheduler.lastCommandRuntimeMs(parkCommand),
                m_scheduler.totalRuntimeMs(parkCommand),
                m_scheduler.runId(parkCommand), // id
                m_scheduler.runId(c3Command) // parent
                ),
        messageJson);
  }

  @Test
  void siblingsInCompositionCanShareRequirements() {
    var resource = new RequireableResource("The Resource", m_scheduler);
    var firstRan = new AtomicBoolean(false);
    var secondRan = new AtomicBoolean(false);

    var first =
        resource
            .run(
                c -> {
                  firstRan.set(true);
                  c.park();
                })
            .named("First");

    var second =
        resource
            .run(
                c -> {
                  secondRan.set(true);
                  c.park();
                })
            .named("Second");

    var group =
        Command.noRequirements(
                co -> {
                  co.fork(first);
                  co.fork(second);
                  co.park();
                })
            .named("Group");

    m_scheduler.schedule(group);
    m_scheduler.run();

    assertTrue(firstRan.get(), "First child should have run to a yield point");
    assertTrue(secondRan.get(), "Second child should have run to a yield point");
    assertFalse(
        m_scheduler.isScheduledOrRunning(first), "First child should have been interrupted");
    assertTrue(m_scheduler.isRunning(second), "Second child should still be running");
    assertTrue(m_scheduler.isRunning(group), "Group should still be running");
  }

  @Test
  void nestedOneShotCompositionsAllRunInOneCycle() {
    var runs = new AtomicInteger(0);
    Supplier<Command> makeOneShot =
        () -> Command.noRequirements(_c -> runs.incrementAndGet()).named("One Shot");
    var command =
        Command.noRequirements(
                co -> {
                  co.fork(makeOneShot.get());
                  co.fork(makeOneShot.get());
                  co.fork(
                      Command.noRequirements(inner -> inner.fork(makeOneShot.get()))
                          .named("Inner"));
                  co.fork(
                      Command.noRequirements(
                              co2 -> {
                                co2.fork(makeOneShot.get());
                                co2.fork(
                                    Command.noRequirements(
                                            co3 -> {
                                              co3.fork(makeOneShot.get());
                                            })
                                        .named("3"));
                              })
                          .named("2"));
                })
            .named("Command");

    m_scheduler.schedule(command);
    m_scheduler.run();
    assertEquals(5, runs.get(), "All oneshot commands should have run");
    assertFalse(m_scheduler.isRunning(command), "Command should have exited after one cycle");
  }

  @Test
  void childConflictsWithHigherPriorityTopLevel() {
    var resource = new RequireableResource("Resource", m_scheduler);
    var top = resource.run(Coroutine::park).withPriority(10).named("Top");

    // Child conflicts with and is lower priority than the Top command
    // It should not be scheduled, and the parent command should exit immediately
    var child = resource.run(Coroutine::park).named("Child");
    var parent = Command.noRequirements(co -> co.await(child)).named("Parent");

    m_scheduler.schedule(top);
    m_scheduler.schedule(parent);
    m_scheduler.run();

    assertTrue(m_scheduler.isRunning(top), "Top command should not have been interrupted");
    assertFalse(m_scheduler.isRunning(child), "Conflicting child should not have run");
    assertFalse(m_scheduler.isRunning(parent), "Parent of conflicting child should have exited");
  }

  @Test
  void childConflictsWithLowerPriorityTopLevel() {
    var resource = new RequireableResource("Resource", m_scheduler);
    var top = resource.run(Coroutine::park).withPriority(-10).named("Top");

    // Child conflicts with and is lower priority than the Top command
    // It should not be scheduled, and the parent command should exit immediately
    var child = resource.run(Coroutine::park).named("Child");
    var parent = Command.noRequirements(co -> co.await(child)).named("Parent");

    m_scheduler.schedule(top);
    m_scheduler.schedule(parent);
    m_scheduler.run();

    assertFalse(m_scheduler.isRunning(top), "Top command should have been interrupted");
    assertTrue(m_scheduler.isRunning(child), "Conflicting child should be running");
    assertTrue(m_scheduler.isRunning(parent), "Parent of conflicting child should be running");
  }

  @Test
  void commandAwaitingItself() {
    // This command deadlocks on itself. It's calling yield() in an infinite loop, which is
    // equivalent to calling Coroutine.park(). No deleterious side effects other than stalling
    // the command
    AtomicReference<Command> commandRef = new AtomicReference<>();
    var command = Command.noRequirements(co -> co.await(commandRef.get())).named("Self Await");
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
    // Externally cancelling command2 allows command1 to continue
    // Externally cancelling command1 cancels both
    var command1 = Command.noRequirements(co -> co.await(ref2.get())).named("Command 1");
    var command2 = Command.noRequirements(co -> co.await(ref1.get())).named("Command 2");
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

  record PriorityCommand(int priority, RequireableResource... subsystems) implements Command {
    @Override
    public void run(Coroutine coroutine) {
      coroutine.park();
    }

    @Override
    public Set<RequireableResource> requirements() {
      return Set.of(subsystems);
    }

    @Override
    public String name() {
      return toString();
    }

    @Override
    public String toString() {
      return "PriorityCommand[priority=" + priority + ", subsystems=" + Set.of(subsystems) + "]";
    }
  }
}
