// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;

class SchedulerTest extends CommandTestBase {
  @Test
  void schedulerLambdaTestNoInterrupt() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Counter counter = new Counter();

      scheduler.onCommandInitialize(command -> counter.increment());
      scheduler.onCommandExecute(command -> counter.increment());
      scheduler.onCommandFinish(command -> counter.increment());

      scheduler.schedule(new InstantCommand());
      scheduler.run();

      assertEquals(counter.m_counter, 3);
    }
  }

  @Test
  void schedulerInterruptLambdaTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Counter counter = new Counter();

      scheduler.onCommandInterrupt(command -> counter.increment());

      Command command = new WaitCommand(10);

      scheduler.schedule(command);
      scheduler.cancel(command);

      assertEquals(counter.m_counter, 1);
    }
  }

  @Test
  void unregisterSubsystemTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Subsystem system = new TestSubsystem();

      scheduler.registerSubsystem(system);
      assertDoesNotThrow(() -> scheduler.unregisterSubsystem(system));
    }
  }

  @Test
  void schedulerCancelAllTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Counter counter = new Counter();

      scheduler.onCommandInterrupt(command -> counter.increment());

      Command command = new WaitCommand(10);
      Command command2 = new WaitCommand(10);

      scheduler.schedule(command);
      scheduler.schedule(command2);
      scheduler.cancelAll();

      assertEquals(counter.m_counter, 2);
    }
  }

  @Test
  void disabledInitTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger(0);
      Subsystem subsystem = new Subsystem() {
        @Override
        public void disabledInit() {
          counter.incrementAndGet();
        }
      };
      scheduler.registerSubsystem(subsystem);

      DriverStationSim.setEnabled(false);
      DriverStationSim.notifyNewData();
      scheduler.run();
      assertEquals(1, counter.get(), "disabledInit() either didn't run");

      scheduler.run();
      assertEquals(1, counter.get(), "disabledInit() ran twice on same disable");

      DriverStationSim.setEnabled(true);
      DriverStationSim.notifyNewData();
      scheduler.run();
      assertEquals(1, counter.get(), "disabledInit() ran when enabled");

      DriverStationSim.setEnabled(false);
      DriverStationSim.notifyNewData();
      scheduler.run();
      assertEquals(2, counter.get(), "disabledInit() didn't run on re-disable");
    }
  }
}
