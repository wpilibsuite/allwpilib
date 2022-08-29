// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;

class RepeatCommandTest {
  @Test
  void callsMethodsCorrectly() {
    HAL.initialize(500, 0);
    // enable so that we don't need to mess with `runsWhenDisabled` for each command
    DriverStationSim.setEnabled(true);

    var initCounter = new AtomicInteger(0);
    var exeCounter = new AtomicInteger(0);
    var isFinishedCounter = new AtomicInteger(0);
    var endCounter = new AtomicInteger(0);
    var isFinishedHook = new AtomicBoolean(false);

    final var command =
            new FunctionalCommand(
                initCounter::incrementAndGet,
                exeCounter::incrementAndGet,
                interrupted -> endCounter.incrementAndGet(),
                () -> {
                  isFinishedCounter.incrementAndGet();
                  return isFinishedHook.get();
                }).repeatedly();

    assertEquals(0, initCounter.get());
    assertEquals(0, exeCounter.get());
    assertEquals(0, isFinishedCounter.get());
    assertEquals(0, endCounter.get());

    CommandScheduler.getInstance().schedule(command);
    assertEquals(1, initCounter.get());
    assertEquals(0, exeCounter.get());
    assertEquals(0, isFinishedCounter.get());
    assertEquals(0, endCounter.get());

    isFinishedHook.set(false);
    CommandScheduler.getInstance().run();
    assertEquals(1, initCounter.get());
    assertEquals(1, exeCounter.get());
    assertEquals(1, isFinishedCounter.get());
    assertEquals(0, endCounter.get());

    isFinishedHook.set(true);
    CommandScheduler.getInstance().run();
    assertEquals(2, initCounter.get());
    assertEquals(2, exeCounter.get());
    assertEquals(2, isFinishedCounter.get());
    assertEquals(1, endCounter.get());

    isFinishedHook.set(false);
    CommandScheduler.getInstance().run();
    assertEquals(2, initCounter.get());
    assertEquals(3, exeCounter.get());
    assertEquals(3, isFinishedCounter.get());
    assertEquals(1, endCounter.get());
  }
}
