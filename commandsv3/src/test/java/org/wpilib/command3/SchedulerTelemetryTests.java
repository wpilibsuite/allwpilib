// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.command3.proto.ProtobufCommands.ProtobufCommand;

class SchedulerTelemetryTests extends CommandTestBase {
  private static final double EPSILON = 1e-9;

  @Test
  void protobuf() {
    var mech = new DummyMechanism("The mechanism", m_scheduler);
    var parkCommand = mech.run(Coroutine::park).named("Park");
    var c3Command = mech.run(co -> co.await(parkCommand)).named("C3");
    var c2Command = mech.run(co -> co.await(c3Command)).named("C2");
    var group = mech.run(co -> co.await(c2Command)).named("Group");

    m_scheduler.schedule(group);
    m_scheduler.run();

    var scheduledCommand1 = Command.noRequirements(Coroutine::park).named("Command 1");
    var scheduledCommand2 = Command.noRequirements(Coroutine::park).named("Command 2");
    m_scheduler.schedule(scheduledCommand1);
    m_scheduler.schedule(scheduledCommand2);

    var message = Scheduler.proto.createMessage();
    Scheduler.proto.pack(message, m_scheduler);

    assertAll(
        () -> assertEquals(m_scheduler.lastRuntimeMs(), message.getLastTimeMs(), EPSILON),
        () -> assertEquals(2, message.getQueuedCommands().length()),
        () -> assertEquals(4, message.getRunningCommands().length()));

    assertCommand(message.getQueuedCommands().get(0), scheduledCommand1, null);
    assertCommand(message.getQueuedCommands().get(1), scheduledCommand2, null);
    assertCommand(message.getRunningCommands().get(0), group, null, "The mechanism");
    assertCommand(message.getRunningCommands().get(1), c2Command, group, "The mechanism");
    assertCommand(message.getRunningCommands().get(2), c3Command, c2Command, "The mechanism");
    assertCommand(message.getRunningCommands().get(3), parkCommand, c3Command, "The mechanism");
  }

  private void assertCommand(ProtobufCommand message, Command command, Command parent) {
    assertCommand(message, command, parent, new String[0]);
  }

  private void assertCommand(
      ProtobufCommand message, Command command, Command parent, String... requirements) {
    var commandName = command.name();

    assertAll(
        commandName,
        () -> assertEquals(m_scheduler.runId(command), message.getId(), "id"),
        () -> assertEquals(commandName, message.getName(), "name"),
        () -> assertEquals(command.priority(), message.getPriority(), "priority"),
        () ->
            assertEquals(requirements.length, message.getRequirements().length(), "requirements"));

    if (parent == null) {
      assertFalse(message.hasParentId(), commandName + " parent ID");
    } else {
      assertEquals(m_scheduler.runId(parent), message.getParentId(), commandName + " parent ID");
    }

    for (int i = 0; i < requirements.length; i++) {
      assertEquals(
          requirements[i],
          message.getRequirements().get(i).getName(),
          commandName + " requirement " + i);
    }

    if (m_scheduler.isRunning(command)) {
      assertAll(
          commandName + " timing",
          () -> assertTrue(message.hasLastTimeMs(), "lastTimeMs"),
          () -> assertTrue(message.hasTotalTimeMs(), "totalTimeMs"),
          () ->
              assertEquals(
                  m_scheduler.lastCommandRuntimeMs(command),
                  message.getLastTimeMs(),
                  EPSILON,
                  "lastTimeMs"),
          () ->
              assertEquals(
                  m_scheduler.totalRuntimeMs(command),
                  message.getTotalTimeMs(),
                  EPSILON,
                  "totalTimeMs"));
    } else {
      assertAll(
          commandName + " timing",
          () -> assertFalse(message.hasLastTimeMs(), "lastTimeMs"),
          () -> assertFalse(message.hasTotalTimeMs(), "totalTimeMs"));
    }
  }
}
