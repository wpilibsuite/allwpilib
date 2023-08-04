// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;

class TimesliceRobotTest {
  static class MockRobot extends TimesliceRobot {
    public final AtomicInteger m_robotPeriodicCount = new AtomicInteger(0);

    MockRobot() {
      super(0.002, 0.005);
    }

    @Override
    public void robotPeriodic() {
      m_robotPeriodicCount.addAndGet(1);
    }
  }

  @BeforeEach
  void setup() {
    SimHooks.pauseTiming();
  }

  @AfterEach
  void cleanup() {
    SimHooks.resumeTiming();
  }

  @Test
  @ResourceLock("timing")
  void scheduleTest() {
    MockRobot robot = new MockRobot();

    final AtomicInteger callbackCount1 = new AtomicInteger(0);
    final AtomicInteger callbackCount2 = new AtomicInteger(0);

    // Timeslice allocation table
    //
    // |       Name      | Offset (ms) | Allocation (ms)|
    // |-----------------|-------------|----------------|
    // | RobotPeriodic() |           0 |              2 |
    // | Callback 1      |           2 |            0.5 |
    // | Callback 2      |         2.5 |              1 |
    robot.schedule(
        () -> {
          callbackCount1.addAndGet(1);
        },
        0.0005);
    robot.schedule(
        () -> {
          callbackCount2.addAndGet(1);
        },
        0.001);

    Thread robotThread =
        new Thread(
            () -> {
              robot.startCompetition();
            });
    robotThread.start();

    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(0.0); // Wait for Notifiers

    // Functions scheduled with addPeriodic() are delayed by one period before
    // their first run (5 ms for this test's callbacks here and 20 ms for
    // robotPeriodic()).
    SimHooks.stepTiming(0.005);

    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(0, callbackCount1.get());
    assertEquals(0, callbackCount2.get());

    // Step to 1.5 ms
    SimHooks.stepTiming(0.0015);
    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(0, callbackCount1.get());
    assertEquals(0, callbackCount2.get());

    // Step to 2.25 ms
    SimHooks.stepTiming(0.00075);
    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(1, callbackCount1.get());
    assertEquals(0, callbackCount2.get());

    // Step to 2.75 ms
    SimHooks.stepTiming(0.0005);
    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(1, callbackCount1.get());
    assertEquals(1, callbackCount2.get());

    robot.endCompetition();
    try {
      robotThread.interrupt();
      robotThread.join();
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    robot.close();
  }

  @Test
  @ResourceLock("timing")
  void scheduleOverrunTest() {
    MockRobot robot = new MockRobot();

    robot.schedule(() -> {}, 0.0005);
    robot.schedule(() -> {}, 0.001);

    // offset = 2 ms + 0.5 ms + 1 ms = 3.5 ms
    // 3.5 ms + 3 ms allocation = 6.5 ms > max of 5 ms
    assertThrows(IllegalStateException.class, () -> robot.schedule(() -> {}, 0.003));

    robot.endCompetition();
    robot.close();
  }
}
