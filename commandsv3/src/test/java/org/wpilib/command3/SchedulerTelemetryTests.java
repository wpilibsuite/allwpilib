// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class SchedulerTelemetryTests extends CommandTestBase {
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
            "name": "Command 1",
            "requirements": []
          }, {
            "priority": 0,
            "id": %s,
            "name": "Command 2",
            "requirements": []
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
}
