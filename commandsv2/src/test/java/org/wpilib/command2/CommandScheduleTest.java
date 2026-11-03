// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command2;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;

import org.junit.jupiter.api.Test;
import org.wpilib.backend.NetworkTablesTunableBackend;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.tunable.MockTunableBackend;
import org.wpilib.tunable.TunableConfig;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.tunable.Tunables;

class CommandScheduleTest extends CommandTestBase {
  @Test
  void instantScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder holder = new MockCommandHolder(true);
      holder.setFinished(true);
      Command mockCommand = holder.getMock();

      scheduler.schedule(mockCommand);
      assertTrue(scheduler.isScheduled(mockCommand));
      verify(mockCommand).initialize();

      scheduler.run();

      verify(mockCommand).execute();
      verify(mockCommand).end(false);

      assertFalse(scheduler.isScheduled(mockCommand));
    }
  }

  @Test
  void singleIterationScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder holder = new MockCommandHolder(true);
      Command mockCommand = holder.getMock();

      scheduler.schedule(mockCommand);

      assertTrue(scheduler.isScheduled(mockCommand));

      scheduler.run();
      holder.setFinished(true);
      scheduler.run();

      verify(mockCommand).initialize();
      verify(mockCommand, times(2)).execute();
      verify(mockCommand).end(false);

      assertFalse(scheduler.isScheduled(mockCommand));
    }
  }

  @Test
  void multiScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();
      MockCommandHolder command3Holder = new MockCommandHolder(true);
      Command command3 = command3Holder.getMock();

      scheduler.schedule(command1, command2, command3);
      assertTrue(scheduler.isScheduled(command1, command2, command3));
      scheduler.run();
      assertTrue(scheduler.isScheduled(command1, command2, command3));

      command1Holder.setFinished(true);
      scheduler.run();
      assertTrue(scheduler.isScheduled(command2, command3));
      assertFalse(scheduler.isScheduled(command1));

      command2Holder.setFinished(true);
      scheduler.run();
      assertTrue(scheduler.isScheduled(command3));
      assertFalse(scheduler.isScheduled(command1, command2));

      command3Holder.setFinished(true);
      scheduler.run();
      assertFalse(scheduler.isScheduled(command1, command2, command3));
    }
  }

  @Test
  void schedulerCancelTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder holder = new MockCommandHolder(true);
      Command mockCommand = holder.getMock();

      scheduler.schedule(mockCommand);

      scheduler.run();
      scheduler.cancel(mockCommand);
      scheduler.run();

      verify(mockCommand).execute();
      verify(mockCommand).end(true);
      verify(mockCommand, never()).end(false);

      assertFalse(scheduler.isScheduled(mockCommand));
    }
  }

  @Test
  void notScheduledCancelTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder holder = new MockCommandHolder(true);
      Command mockCommand = holder.getMock();

      assertDoesNotThrow(() -> scheduler.cancel(mockCommand));
    }
  }

  @Test
  void tunableCancelTest() {
    var backend = new MockTunableBackend();
    TunableRegistry.registerBackend("", backend);
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Tunables.publish("Scheduler", scheduler);

      var namesTunable = backend.getTunable("/Scheduler/Names");
      assertFalse(namesTunable.getConfig().isMutable());
      assertEquals(TunableConfig.Polling.ALWAYS_GET, namesTunable.getConfig().getPolling());

      var idsTunable = backend.getTunable("/Scheduler/Ids");
      assertFalse(idsTunable.getConfig().isMutable());
      assertEquals(TunableConfig.Polling.ALWAYS_GET, idsTunable.getConfig().getPolling());

      var cancelTunable = backend.getTunable("/Scheduler/Cancel");
      assertEquals(TunableConfig.Polling.ALWAYS_GET, cancelTunable.getConfig().getPolling());

      MockCommandHolder holder = new MockCommandHolder(true);
      Command mockCommand = holder.getMock();
      scheduler.schedule(mockCommand);
      scheduler.run();
      assertTrue(scheduler.isScheduled(mockCommand));
      assertArrayEquals(
          new String[] {mockCommand.getName()},
          backend.getValue("/Scheduler/Names", String[].class));
      assertArrayEquals(
          new long[] {mockCommand.hashCode()}, backend.getValue("/Scheduler/Ids", long[].class));

      backend.setArray("/Scheduler/Cancel", new long[] {mockCommand.hashCode()});
      TunableRegistry.update();
      scheduler.run();
      assertFalse(scheduler.isScheduled(mockCommand));
    } finally {
      TunableRegistry.reset();
    }
  }

  @Test
  void schedulerCloseRemovesPublishedTunables() {
    var backend = new MockTunableBackend();
    TunableRegistry.registerBackend("", backend);
    try {
      CommandScheduler scheduler = new CommandScheduler();
      Tunables.publish("Scheduler", scheduler);

      assertDoesNotThrow(() -> backend.getTunable("/Scheduler/Names"));
      assertDoesNotThrow(() -> backend.getTunable("/Scheduler/Ids"));
      assertDoesNotThrow(() -> backend.getTunable("/Scheduler/Cancel"));

      scheduler.close();

      assertThrows(IllegalArgumentException.class, () -> backend.getTunable("/Scheduler/Names"));
      assertThrows(IllegalArgumentException.class, () -> backend.getTunable("/Scheduler/Ids"));
      assertThrows(IllegalArgumentException.class, () -> backend.getTunable("/Scheduler/Cancel"));

      try (CommandScheduler replacement = new CommandScheduler()) {
        assertDoesNotThrow(() -> Tunables.publish("Scheduler", replacement));
        assertDoesNotThrow(() -> backend.getTunable("/Scheduler/Names"));
        assertDoesNotThrow(() -> backend.getTunable("/Scheduler/Ids"));
        assertDoesNotThrow(() -> backend.getTunable("/Scheduler/Cancel"));
      }
    } finally {
      TunableRegistry.reset();
    }
  }

  @Test
  void networkTablesTunableCancelUsesRobustTuneTopic() {
    try (NetworkTableInstance inst = NetworkTableInstance.create()) {
      TunableRegistry.registerBackend("", new NetworkTablesTunableBackend(inst, "/Tunables"));
      try (CommandScheduler scheduler = new CommandScheduler()) {
        Tunables.publish("Scheduler", scheduler);

        assertEquals(
            "true", inst.getTopic("/Tunables/Scheduler/Cancel/value").getProperty("robust"));
        var cancelValue =
            inst.getIntegerArrayTopic("/Tunables/Scheduler/Cancel/value").subscribe(new long[] {});
        assertArrayEquals(new long[] {}, cancelValue.get());

        MockCommandHolder holder = new MockCommandHolder(true);
        Command mockCommand = holder.getMock();
        scheduler.schedule(mockCommand);
        scheduler.run();
        assertTrue(scheduler.isScheduled(mockCommand));

        inst.getIntegerArrayTopic("/Tunables/Scheduler/Cancel/tune")
            .publish()
            .set(new long[] {mockCommand.hashCode()});
        inst.flush();
        TunableRegistry.update();
        scheduler.run();
        assertFalse(scheduler.isScheduled(mockCommand));
        assertArrayEquals(new long[] {}, cancelValue.get());
      } finally {
        TunableRegistry.reset();
      }
    }
  }
}
