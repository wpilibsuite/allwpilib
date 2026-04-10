// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import org.junit.jupiter.api.Test;

class SchedulerDefaultCommandTests extends CommandTestBase {
  @Test
  void globalDefaultCommandIsAlwaysUsed() {
    var mech = new Mechanism("Mech", m_scheduler);
    var defaultCommand = mech.run(Coroutine::park).named("Custom Default Command");

    mech.setDefaultCommand(defaultCommand);
    assertFalse(m_scheduler.isScheduledOrRunning(defaultCommand));

    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(defaultCommand));

    m_opModeId = 1;
    m_opModeName = "Opmode 1";

    m_scheduler.run();
    assertTrue(
        m_scheduler.isRunning(defaultCommand),
        "Default command should still be running when the opmode changes");
  }

  @Test
  void defaultCommandSetInOpmodeStops() {
    var mech = new Mechanism("Mech", m_scheduler);
    var initialDefaultCommand = mech.idle();
    mech.setDefaultCommand(initialDefaultCommand);

    m_opModeId = 1;
    m_opModeName = "Opmode 1";

    var opModeScopedCommand = mech.run(Coroutine::park).named("Custom Default Command");
    mech.setDefaultCommand(opModeScopedCommand);

    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(opModeScopedCommand));

    // Change opmode. The globally scoped default command should run
    m_opModeId = 2;
    m_opModeName = "Opmode 2";
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(opModeScopedCommand));
    assertTrue(m_scheduler.isRunning(initialDefaultCommand));
  }

  @Test
  void defaultCommandSetInCommandStops() {
    var mech = new Mechanism("Mech", m_scheduler);
    var initialDefaultCommand = mech.idle();
    mech.setDefaultCommand(initialDefaultCommand);

    var commandScopedCommand = mech.run(Coroutine::park).named("Command-Scoped Default Command");

    var scopingCommand =
        Command.noRequirements()
            .executing(
                co -> {
                  mech.setDefaultCommand(commandScopedCommand);
                  co.park();
                })
            .named("Scoping Command");

    m_scheduler.schedule(scopingCommand);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(scopingCommand));
    assertTrue(m_scheduler.isRunning(commandScopedCommand));
    assertFalse(m_scheduler.isRunning(initialDefaultCommand));

    m_scheduler.cancel(scopingCommand);
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(scopingCommand));
    assertFalse(m_scheduler.isRunning(commandScopedCommand));
    assertTrue(m_scheduler.isRunning(initialDefaultCommand));
  }

  @Test
  void interruptingDefaultCommandInterruptsOwner() {
    var mech = new Mechanism("Mech", m_scheduler);
    var initialDefaultCommand = mech.idle();
    mech.setDefaultCommand(initialDefaultCommand);

    var commandScopedCommand = mech.run(Coroutine::park).named("Command-Scoped Default Command");
    var scopingCommand =
        Command.noRequirements()
            .executing(
                co -> {
                  mech.setDefaultCommand(commandScopedCommand);
                  co.park();
                })
            .named("Scoping Command");

    final Command interruptor = mech.run(Coroutine::park).named("Interruptor");
    m_scheduler.schedule(scopingCommand);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(scopingCommand));
    assertTrue(m_scheduler.isRunning(commandScopedCommand));
    assertFalse(m_scheduler.isRunning(initialDefaultCommand));

    m_scheduler.schedule(interruptor);
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(scopingCommand));
    assertFalse(m_scheduler.isRunning(commandScopedCommand));
    assertFalse(m_scheduler.isRunning(initialDefaultCommand));
    assertTrue(m_scheduler.isRunning(interruptor));
  }

  @Test
  void defaultCommandStackup() {
    var mech = new Mechanism("Mech", m_scheduler);
    var initialDefaultCommand = mech.idle();
    mech.setDefaultCommand(initialDefaultCommand);

    var opModeScopedCommand = mech.run(Coroutine::park).named("Opmode Scoped Default Command");
    var commandScopedCommand = mech.run(Coroutine::park).named("Command-Scoped Default Command");

    final Command scopingCommand =
        Command.noRequirements()
            .executing(
                co -> {
                  mech.setDefaultCommand(commandScopedCommand);
                  co.park();
                })
            .named("Scoping Command");

    m_opModeId = 1;
    m_opModeName = "Opmode 1";
    mech.setDefaultCommand(opModeScopedCommand);

    m_scheduler.run();
    assertTrue(
        m_scheduler.isRunning(opModeScopedCommand),
        "Opmode-scoped default command should be active");

    m_scheduler.schedule(scopingCommand);
    m_scheduler.run();
    assertFalse(
        m_scheduler.isRunning(opModeScopedCommand),
        "Opmode-scoped default command should have stopped");
    assertTrue(m_scheduler.isRunning(commandScopedCommand));

    m_events.clear();
    m_opModeId = 2;
    m_opModeName = "Opmode 2";
    m_scheduler.run();

    assertFalse(
        m_scheduler.isRunning(opModeScopedCommand),
        "Opmode-scoped default command should have stopped when the opmode changed");
    assertFalse(
        m_scheduler.isRunning(scopingCommand),
        "Running opmode-scoped command should have stopped when the opmode changed");
    assertFalse(
        m_scheduler.isRunning(commandScopedCommand),
        "Command-scoped default command should have stopped when its parent command stopped");
    assertTrue(
        m_scheduler.isRunning(initialDefaultCommand), "Global default command should be active");
  }

  @Test
  void defaultCommandChangingDefaultCommand() {
    var mech =
        new Mechanism("Mech", m_scheduler) {
          Command makeCommand1() {
            return run(co -> {
                  setDefaultCommand(makeCommand2());
                  // One-shot default commands won't appear in the `getRunningCommandsFor` list,
                  // We need to park the coroutine so we can observe the change
                  co.park();
                })
                .named("Command 1");
          }

          Command makeCommand2() {
            return run(co -> {
                  setDefaultCommand(makeCommand1());
                  co.park();
                })
                .named("Command 2");
          }
        };

    mech.setDefaultCommand(mech.makeCommand1());

    // Command 1 and Command 2 should alternate as default commands
    for (int i = 0; i < 10; i++) {
      m_scheduler.run();

      String expectedDefaultCommand = i % 2 == 0 ? "Command 1" : "Command 2";
      assertEquals(
          List.of(expectedDefaultCommand),
          m_scheduler.getRunningCommandsFor(mech).stream().map(Command::name).toList());
    }

    // Check for memory leaks. If every call to `setDefaultCommand` were to add a new binding,
    // we'd have 1 binding object per loop and quickly use a ton of memory
    var defaultCommandBindings = m_scheduler.getDefaultCommandBindingsFor(mech);
    assertEquals(
        List.of("Mech[IDLE]", "Command 1", "Command 2", "Command 1"),
        defaultCommandBindings.stream().map(b -> b.command().name()).toList());

    m_scheduler.run();
    assertEquals(
        List.of("Mech[IDLE]", "Command 1", "Command 2"),
        defaultCommandBindings.stream().map(b -> b.command().name()).toList());
  }
}
