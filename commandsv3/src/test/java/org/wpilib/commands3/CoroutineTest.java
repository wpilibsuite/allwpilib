// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class CoroutineTest {
  Scheduler scheduler;
  ContinuationScope scope;

  @BeforeEach
  void setup() {
    scheduler = new Scheduler();
    scope = new ContinuationScope("Test Scope");
  }


  @Test
  void forkMany() {
    var a = new NullCommand();
    var b = new NullCommand();
    var c = new NullCommand();

    var all = Command.noRequirements(co -> {
      co.fork(a, b, c);
      co.park();
    }).named("Fork Many");

    scheduler.schedule(all);
    scheduler.run();
    assertTrue(scheduler.isRunning(a));
    assertTrue(scheduler.isRunning(b));
    assertTrue(scheduler.isRunning(c));
  }
}
