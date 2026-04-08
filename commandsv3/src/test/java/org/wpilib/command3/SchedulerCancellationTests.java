// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import org.junit.jupiter.api.Test;

class SchedulerCancellationTests extends CommandTestBase {
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
  void defaultCommandResumesAfterInterruption() {
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
    assertTrue(m_scheduler.isRunning(defaultCmd), "Default command should have resumed");
    assertEquals(2, count.get());
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

    assertFalse(m_scheduler.isRunning(command), "Command was not canceled by cancelAll()");

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
}
