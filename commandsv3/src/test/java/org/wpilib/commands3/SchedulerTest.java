// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static edu.wpi.first.units.Units.Microseconds;
import static edu.wpi.first.units.Units.Milliseconds;
import static edu.wpi.first.units.Units.Seconds;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertThrowsExactly;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;
import static org.wpilib.commands3.SchedulerEvent.Canceled;
import static org.wpilib.commands3.SchedulerEvent.CompletedWithError;
import static org.wpilib.commands3.SchedulerEvent.Mounted;
import static org.wpilib.commands3.SchedulerEvent.Scheduled;
import static org.wpilib.commands3.SchedulerEvent.Yielded;

import edu.wpi.first.wpilibj.RobotController;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Supplier;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;

@Timeout(5)
class SchedulerTest {
  private Scheduler m_scheduler;
  private List<SchedulerEvent> m_events;

  @BeforeEach
  void setup() {
    RobotController.setTimeSource(() -> System.nanoTime() / 1000L);
    m_scheduler = new Scheduler();
    m_events = new ArrayList<>();
    m_scheduler.addEventListener(m_events::add);
  }

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
  void higherPriorityCancels() {
    final var subsystem = new Mechanism("Subsystem", m_scheduler);

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
    final var subsystem = new Mechanism("Subsystem", m_scheduler);

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
    final var subsystem = new Mechanism("Subsystem", m_scheduler);

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
    var mechanism =
        new Mechanism("X", m_scheduler) {
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
                      coroutine.yield();
                      mechanism.m_x++;
                    }
                  })
              .named("CountCommand[" + cmdCount + "]");

      m_scheduler.schedule(command);
    }

    for (int i = 0; i < iterations; i++) {
      m_scheduler.run();
    }
    m_scheduler.run();

    assertEquals(numCommands * iterations, mechanism.m_x);
  }

  @Test
  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  void errorDetection() {
    var mechanism = new Mechanism("X", m_scheduler);

    var command =
        mechanism
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
        Command.noRequirements()
            .executing(
                co -> {
                  co.await(
                      Command.noRequirements()
                          .executing(
                              c2 -> {
                                new Trigger(m_scheduler, () -> true)
                                    .onTrue(
                                        Command.noRequirements()
                                            .executing(
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
  void commandEncounteringErrorCancelsChildren() {
    var child = Command.noRequirements().executing(Coroutine::park).named("Child 1");
    var command =
        Command.noRequirements()
            .executing(
                co -> {
                  co.fork(child);
                  throw new RuntimeException("The exception");
                })
            .named("Bad Behavior");

    m_scheduler.schedule(command);
    assertThrows(RuntimeException.class, m_scheduler::run);
    assertFalse(
        m_scheduler.isScheduledOrRunning(command),
        "Command should have been removed from the scheduler");
    assertFalse(
        m_scheduler.isScheduledOrRunning(child),
        "Child should have been removed from the scheduler");
  }

  @Test
  void childCommandEncounteringErrorCancelsParent() {
    var child =
        Command.noRequirements()
            .executing(
                co -> {
                  throw new RuntimeException("The exception"); // note: bubbles up to the parent
                })
            .named("Child 1");
    var command =
        Command.noRequirements()
            .executing(
                co -> {
                  co.await(child);
                  co.park(); // pretend other things would happen after the child
                })
            .named("Parent");

    m_scheduler.schedule(command);
    assertThrows(RuntimeException.class, m_scheduler::run);
    assertFalse(
        m_scheduler.isRunning(command),
        "Parent command should have been removed from the scheduler");
    assertFalse(m_scheduler.isRunning(child), "Child should have been removed from the scheduler");
  }

  @Test
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  void childCommandEncounteringErrorAfterRemountCancelsParent() {
    var child =
        Command.noRequirements()
            .executing(
                co -> {
                  co.yield();
                  throw new RuntimeException("The exception"); // does not bubble up to the parent
                })
            .named("Child 1");
    var command =
        Command.noRequirements()
            .executing(
                co -> {
                  co.await(child);
                  co.park(); // pretend other things would happen after the child
                })
            .named("Parent");

    m_scheduler.schedule(command);

    // first run schedules the child and adds it to the running set
    m_scheduler.run();

    // second run encounters the error in the child
    final var error = assertThrows(RuntimeException.class, m_scheduler::run);
    assertFalse(
        m_scheduler.isRunning(command),
        "Parent command should have been removed from the scheduler");
    assertFalse(m_scheduler.isRunning(child), "Child should have been removed from the scheduler");

    // Full event history
    assertEquals(9, m_events.size());
    assertTrue(
        m_events.get(0) instanceof Scheduled s && s.command() == command,
        "First event should be parent scheduled");
    assertTrue(
        m_events.get(1) instanceof Mounted m && m.command() == command,
        "Second event should be parent mounted");
    assertTrue(
        m_events.get(2) instanceof Scheduled s && s.command() == child,
        "Third event should be child scheduled");
    assertTrue(
        m_events.get(3) instanceof Mounted m && m.command() == child,
        "Fourth event should be child mounted");
    assertTrue(
        m_events.get(4) instanceof Yielded y && y.command() == child,
        "Fifth event should be child yielded");
    assertTrue(
        m_events.get(5) instanceof Yielded y && y.command() == command,
        "Sixth event should be parent yielded");
    assertTrue(
        m_events.get(6) instanceof Mounted m && m.command() == child,
        "Seventh event should be child remounted");
    assertTrue(
        m_events.get(7) instanceof CompletedWithError c
            && c.command() == child
            && c.error() == error,
        "Eighth event should be child completed with error");
    assertTrue(
        m_events.get(8) instanceof Canceled c && c.command() == command,
        "Ninth event should be parent canceled");
  }

  @Test
  @SuppressWarnings({"PMD.RedundantFieldInitializer", "PMD.ImmutableField"}) // PMD bugs
  void runMechanism() {
    var example =
        new Mechanism("Counting", m_scheduler) {
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

    var mechanism = new Mechanism("mechanism", m_scheduler);

    var interrupter =
        Command.requiring(mechanism)
            .executing(coroutine -> {})
            .withPriority(2)
            .named("Interrupter");

    var canceledCommand =
        Command.requiring(mechanism)
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

    var mechanism = new Mechanism("mechanism", m_scheduler);
    var defaultCmd =
        mechanism
            .run(
                coroutine -> {
                  while (true) {
                    count.incrementAndGet();
                    coroutine.yield();
                  }
                })
            .withPriority(-1)
            .named("Default Command");

    final var newerCmd = mechanism.run(coroutine -> {}).named("Newer Command");
    mechanism.setDefaultCommand(defaultCmd);
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
    var command = Command.noRequirements().executing(Coroutine::park).named("Command");
    m_scheduler.schedule(command);
    m_scheduler.cancel(command);
    assertFalse(m_scheduler.isScheduledOrRunning(command));
  }

  @Test
  void commandCancelingSelf() {
    var ranAfterCancel = new AtomicBoolean(false);
    var commandRef = new AtomicReference<Command>(null);
    var command =
        Command.noRequirements()
            .executing(
                co -> {
                  co.scheduler().cancel(commandRef.get());
                  ranAfterCancel.set(true);
                })
            .named("Command");
    commandRef.set(command);
    m_scheduler.schedule(command);

    var error = assertThrows(IllegalArgumentException.class, () -> m_scheduler.run());
    assertEquals("Command `Command` is mounted and cannot be canceled", error.getMessage());
    assertFalse(ranAfterCancel.get(), "Command should have stopped after encountering an error");
    assertFalse(
        m_scheduler.isScheduledOrRunning(command),
        "Command should have been removed from the scheduler");
  }

  @Test
  void cancelAllEvictsOnDeck() {
    var command = Command.noRequirements().executing(Coroutine::park).named("Command");
    m_scheduler.schedule(command);
    m_scheduler.cancelAll();
    assertFalse(m_scheduler.isScheduledOrRunning(command));
  }

  @Test
  void cancelAllCancelsAll() {
    var commands = new ArrayList<Command>(10);
    for (int i = 1; i <= 10; i++) {
      commands.add(Command.noRequirements().executing(Coroutine::yield).named("Command " + i));
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
  void cancelAllCallsOnCancelHookForRunningCommands() {
    AtomicBoolean ranHook = new AtomicBoolean(false);
    var command =
        Command.noRequirements()
            .executing(Coroutine::park)
            .whenCanceled(() -> ranHook.set(true))
            .named("Command");
    m_scheduler.schedule(command);
    m_scheduler.run();
    m_scheduler.cancelAll();
    assertTrue(ranHook.get(), "onCancel hook was not called");
  }

  @Test
  void cancelAllDoesNotCallOnCancelHookForQueuedCommands() {
    AtomicBoolean ranHook = new AtomicBoolean(false);
    var command =
        Command.noRequirements()
            .executing(Coroutine::park)
            .whenCanceled(() -> ranHook.set(true))
            .named("Command");
    m_scheduler.schedule(command);
    // no call to run before cancelAll()
    m_scheduler.cancelAll();
    assertFalse(ranHook.get(), "onCancel hook was not called");
  }

  @Test
  void cancelAllStartsDefaults() {
    var mechanisms = new ArrayList<Mechanism>(10);
    for (int i = 1; i <= 10; i++) {
      mechanisms.add(new Mechanism("System " + i, m_scheduler));
    }

    var command = Command.requiring(mechanisms).executing(Coroutine::yield).named("Big Command");

    // Scheduling the command should evict the on-deck default commands
    m_scheduler.schedule(command);

    // Then running should get it into the set of running commands
    m_scheduler.run();

    // Canceling should clear out the set of running commands
    m_scheduler.cancelAll();

    // Then ticking the scheduler once to fully remove the command and schedule the defaults
    m_scheduler.run();

    // Then one more tick to start running the scheduled defaults
    m_scheduler.run();

    if (m_scheduler.isRunning(command)) {
      System.err.println(m_scheduler.getRunningCommands());
      fail(command.name() + " was not canceled by cancelAll()");
    }

    for (var mechanism : mechanisms) {
      var runningCommands = m_scheduler.getRunningCommandsFor(mechanism);
      assertEquals(
          1,
          runningCommands.size(),
          "mechanism " + mechanism + " should have exactly one running command");
      assertEquals(
          mechanism.getDefaultCommand(),
          runningCommands.getFirst(),
          "mechanism " + mechanism + " is not running the default command");
    }
  }

  @Test
  void cancelDeeplyNestedCompositions() {
    Command root =
        Command.noRequirements()
            .executing(
                co -> {
                  co.await(
                      Command.noRequirements()
                          .executing(
                              co2 -> {
                                co2.await(
                                    Command.noRequirements()
                                        .executing(
                                            co3 -> {
                                              co3.await(
                                                  Command.noRequirements()
                                                      .executing(Coroutine::park)
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
    var mech = new Mechanism("The mechanism", m_scheduler);
    var group =
        mech.run(
                co -> {
                  co.await(
                      mech.run(
                              co2 -> {
                                co2.await(
                                    mech.run(
                                            co3 -> {
                                              co3.await(mech.run(Coroutine::park).named("Park"));
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
    var mech = new Mechanism("The mechanism", m_scheduler);
    var group =
        mech.run(
                co -> {
                  co.fork(mech.run(Coroutine::park).named("First Child"));
                  co.fork(mech.run(Coroutine::park).named("Second Child"));
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
    var m1 = new Mechanism("M1", m_scheduler);
    var m2 = new Mechanism("m2", m_scheduler);

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
  void compositionsCannotAwaitConflictingCommands() {
    var mech = new Mechanism("The Mechanism", m_scheduler);

    var group =
        Command.noRequirements()
            .executing(
                co -> {
                  co.awaitAll(
                      mech.run(Coroutine::park).named("First"),
                      mech.run(Coroutine::park).named("Second"));
                })
            .named("Group");

    m_scheduler.schedule(group);

    // Running should attempt to schedule multiple conflicting commands
    var exception = assertThrows(IllegalArgumentException.class, m_scheduler::run);
    assertEquals(
        "Commands running in parallel cannot share requirements: "
            + "First and Second both require The Mechanism",
        exception.getMessage());
  }

  @Test
  void doesNotRunOnCancelWhenInterruptingOnDeck() {
    var ran = new AtomicBoolean(false);

    var mechanism = new Mechanism("The mechanism", m_scheduler);
    var cmd = mechanism.run(Coroutine::yield).whenCanceled(() -> ran.set(true)).named("cmd");
    var interrupter = mechanism.run(Coroutine::yield).named("Interrupter");
    m_scheduler.schedule(cmd);
    m_scheduler.schedule(interrupter);
    m_scheduler.run();

    assertFalse(ran.get(), "onCancel ran when it shouldn't have!");
  }

  @Test
  void doesNotRunOnCancelWhenCancelingOnDeck() {
    var ran = new AtomicBoolean(false);

    var mechanism = new Mechanism("The mechanism", m_scheduler);
    var cmd = mechanism.run(Coroutine::yield).whenCanceled(() -> ran.set(true)).named("cmd");
    m_scheduler.schedule(cmd);
    // canceling before calling .run()
    m_scheduler.cancel(cmd);
    m_scheduler.run();

    assertFalse(ran.get(), "onCancel ran when it shouldn't have!");
  }

  @Test
  void runsOnCancelWhenInterruptingCommand() {
    var ran = new AtomicBoolean(false);

    var mechanism = new Mechanism("The mechanism", m_scheduler);
    var cmd = mechanism.run(Coroutine::park).whenCanceled(() -> ran.set(true)).named("cmd");
    var interrupter = mechanism.run(Coroutine::park).named("Interrupter");
    m_scheduler.schedule(cmd);
    m_scheduler.run();
    m_scheduler.schedule(interrupter);
    m_scheduler.run();

    assertTrue(ran.get(), "onCancel should have run!");
  }

  @Test
  void doesNotRunOnCancelWhenCompleting() {
    var ran = new AtomicBoolean(false);

    var mechanism = new Mechanism("The mechanism", m_scheduler);
    var cmd = mechanism.run(Coroutine::yield).whenCanceled(() -> ran.set(true)).named("cmd");
    m_scheduler.schedule(cmd);
    m_scheduler.run();
    m_scheduler.run();

    assertFalse(m_scheduler.isScheduledOrRunning(cmd));
    assertFalse(ran.get(), "onCancel ran when it shouldn't have!");
  }

  @Test
  void runsOnCancelWhenCanceling() {
    var ran = new AtomicBoolean(false);

    var mechanism = new Mechanism("The mechanism", m_scheduler);
    var cmd = mechanism.run(Coroutine::yield).whenCanceled(() -> ran.set(true)).named("cmd");
    m_scheduler.schedule(cmd);
    m_scheduler.run();
    m_scheduler.cancel(cmd);

    assertTrue(ran.get(), "onCancel should have run!");
  }

  @Test
  void runsOnCancelWhenCancelingParent() {
    var ran = new AtomicBoolean(false);

    var mechanism = new Mechanism("The mechanism", m_scheduler);
    var cmd = mechanism.run(Coroutine::yield).whenCanceled(() -> ran.set(true)).named("cmd");

    var group = new SequentialGroup("Seq", Collections.singletonList(cmd));
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

  @Test
  void nestedMechanisms() {
    var superstructure =
        new Mechanism("Superstructure", m_scheduler) {
          private final Mechanism m_elevator = new Mechanism("Elevator", m_scheduler);
          private final Mechanism m_arm = new Mechanism("Arm", m_scheduler);

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

  @Test
  void protobuf() {
    var mech = new Mechanism("The mechanism", m_scheduler);
    var parkCommand = mech.run(Coroutine::park).named("Park");
    var c3Command = mech.run(co -> co.await(parkCommand)).named("C3");
    var c2Command = mech.run(co -> co.await(c3Command)).named("C2");
    var group = mech.run(co -> co.await(c2Command)).named("Group");

    m_scheduler.schedule(group);
    m_scheduler.run();

    var scheduledCommand1 = Command.noRequirements().executing(Coroutine::park).named("Command 1");
    var scheduledCommand2 = Command.noRequirements().executing(Coroutine::park).named("Command 2");
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
              "name": "The mechanism"
            }]
          }, {
            "lastTimeMs": %s,
            "totalTimeMs": %s,
            "priority": 0,
            "id": %s,
            "parentId": %s,
            "name": "C2",
            "requirements": [{
              "name": "The mechanism"
            }]
          }, {
            "lastTimeMs": %s,
            "totalTimeMs": %s,
            "priority": 0,
            "id": %s,
            "parentId": %s,
            "name": "C3",
            "requirements": [{
              "name": "The mechanism"
            }]
          }, {
            "lastTimeMs": %s,
            "totalTimeMs": %s,
            "priority": 0,
            "id": %s,
            "parentId": %s,
            "name": "Park",
            "requirements": [{
              "name": "The mechanism"
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
    var mechanism = new Mechanism("The mechanism", m_scheduler);
    var firstRan = new AtomicBoolean(false);
    var secondRan = new AtomicBoolean(false);

    var first =
        mechanism
            .run(
                c -> {
                  firstRan.set(true);
                  c.park();
                })
            .named("First");

    var second =
        mechanism
            .run(
                c -> {
                  secondRan.set(true);
                  c.park();
                })
            .named("Second");

    var group =
        Command.noRequirements()
            .executing(
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
        () -> Command.noRequirements().executing(_c -> runs.incrementAndGet()).named("One Shot");
    var command =
        Command.noRequirements()
            .executing(
                co -> {
                  co.fork(makeOneShot.get());
                  co.fork(makeOneShot.get());
                  co.fork(
                      Command.noRequirements()
                          .executing(inner -> inner.fork(makeOneShot.get()))
                          .named("Inner"));
                  co.fork(
                      Command.noRequirements()
                          .executing(
                              co2 -> {
                                co2.fork(makeOneShot.get());
                                co2.fork(
                                    Command.noRequirements()
                                        .executing(
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
    var mechanism = new Mechanism("mechanism", m_scheduler);
    var top = mechanism.run(Coroutine::park).withPriority(10).named("Top");

    // Child conflicts with and is lower priority than the Top command
    // It should not be scheduled, and the parent command should exit immediately
    var child = mechanism.run(Coroutine::park).named("Child");
    var parent = Command.noRequirements().executing(co -> co.await(child)).named("Parent");

    m_scheduler.schedule(top);
    m_scheduler.schedule(parent);
    m_scheduler.run();

    assertTrue(m_scheduler.isRunning(top), "Top command should not have been interrupted");
    assertFalse(m_scheduler.isRunning(child), "Conflicting child should not have run");
    assertFalse(m_scheduler.isRunning(parent), "Parent of conflicting child should have exited");
  }

  @Test
  void childConflictsWithLowerPriorityTopLevel() {
    var mechanism = new Mechanism("mechanism", m_scheduler);
    var top = mechanism.run(Coroutine::park).withPriority(-10).named("Top");

    // Child conflicts with and is lower priority than the Top command
    // It should not be scheduled, and the parent command should exit immediately
    var child = mechanism.run(Coroutine::park).named("Child");
    var parent = Command.noRequirements().executing(co -> co.await(child)).named("Parent");

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
    m_scheduler.schedule(outer); // schedules inner

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

  record PriorityCommand(int priority, Mechanism... subsystems) implements Command {
    @Override
    public void run(Coroutine coroutine) {
      coroutine.park();
    }

    @Override
    public Set<Mechanism> requirements() {
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
