// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.framework;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.wpilib.driverstation.internal.DriverStationBackend;
import org.wpilib.hardware.hal.OpModeOption;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.opmode.OpMode;
import org.wpilib.simulation.DriverStationSim;
import org.wpilib.simulation.SimHooks;

@ResourceLock("timing")
class OpModeLifecycleTest {
  private static final double kPeriod = 0.02;

  private static long makeOpModeId(RobotMode mode, String name) {
    return OpModeOption.makeId(mode, name.hashCode());
  }

  static class MockOpMode implements OpMode {
    public final AtomicInteger constructedCount;
    public final AtomicInteger disabledPeriodicCount;
    public final AtomicInteger startCount;
    public final AtomicInteger periodicCount;
    public final AtomicInteger endCount;
    public final AtomicInteger closeCount;

    MockOpMode(
        AtomicInteger constructedCount,
        AtomicInteger disabledPeriodicCount,
        AtomicInteger startCount,
        AtomicInteger periodicCount,
        AtomicInteger endCount,
        AtomicInteger closeCount) {
      this.constructedCount = constructedCount;
      this.disabledPeriodicCount = disabledPeriodicCount;
      this.startCount = startCount;
      this.periodicCount = periodicCount;
      this.endCount = endCount;
      this.closeCount = closeCount;
      this.constructedCount.incrementAndGet();
    }

    @Override
    public void disabledPeriodic() {
      disabledPeriodicCount.incrementAndGet();
    }

    @Override
    public void start() {
      startCount.incrementAndGet();
    }

    @Override
    public void periodic() {
      periodicCount.incrementAndGet();
    }

    @Override
    public void end() {
      endCount.incrementAndGet();
    }

    @Override
    public void close() {
      closeCount.incrementAndGet();
    }
  }

  static class LifecycleRobot extends OpModeRobot {
    LifecycleRobot() {
      super();
    }
  }

  @BeforeEach
  void setUp() {
    SimHooks.pauseTiming();
    SimHooks.setProgramStarted(false);
    DriverStationSim.resetData();
    DriverStationSim.setDsAttached(true);
    DriverStationSim.setEnabled(false);
  }

  @AfterEach
  void tearDown() {
    DriverStationBackend.clearOpModes();
    SimHooks.resumeTiming();
  }

  @Test
  void testLifecycleEnabledTransition() throws InterruptedException {
    AtomicInteger constructedCount = new AtomicInteger(0);
    AtomicInteger disabledPeriodicCount = new AtomicInteger(0);
    AtomicInteger startCount = new AtomicInteger(0);
    AtomicInteger periodicCount = new AtomicInteger(0);
    AtomicInteger endCount = new AtomicInteger(0);
    AtomicInteger closeCount = new AtomicInteger(0);

    LifecycleRobot robot = new LifecycleRobot();
    robot.addOpModeFactory(
        () ->
            new MockOpMode(
                constructedCount,
                disabledPeriodicCount,
                startCount,
                periodicCount,
                endCount,
                closeCount),
        RobotMode.TELEOPERATED,
        "TestOpMode");
    robot.publishOpModes();

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();
    SimHooks.waitForProgramStart();

    // 1. Selected, but disabled
    DriverStationSim.setRobotMode(RobotMode.TELEOPERATED);
    DriverStationSim.setOpMode(makeOpModeId(RobotMode.TELEOPERATED, "TestOpMode"));
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(kPeriod);
    assertEquals(1, constructedCount.get());
    assertEquals(1, disabledPeriodicCount.get());

    // 2. Transition to enabled
    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(2 * kPeriod);
    // Starts on first loop
    assertEquals(1, startCount.get());
    // Periodic is called on second loop
    assertEquals(1, periodicCount.get());

    // 3. Transition to disabled
    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(kPeriod);
    assertEquals(1, endCount.get());
    assertEquals(1, closeCount.get());

    robot.endCompetition();
    robotThread.join();
    robot.close();
  }

  @Test
  void testLifecycleOpModeChangeWhileEnabled() throws InterruptedException {
    AtomicInteger constructedCount1 = new AtomicInteger(0);
    AtomicInteger disabledPeriodicCount1 = new AtomicInteger(0);
    AtomicInteger startCount1 = new AtomicInteger(0);
    AtomicInteger periodicCount1 = new AtomicInteger(0);
    AtomicInteger endCount1 = new AtomicInteger(0);
    AtomicInteger closeCount1 = new AtomicInteger(0);

    AtomicInteger constructedCount2 = new AtomicInteger(0);
    AtomicInteger disabledPeriodicCount2 = new AtomicInteger(0);
    AtomicInteger startCount2 = new AtomicInteger(0);
    AtomicInteger periodicCount2 = new AtomicInteger(0);
    AtomicInteger endCount2 = new AtomicInteger(0);
    AtomicInteger closeCount2 = new AtomicInteger(0);

    LifecycleRobot robot = new LifecycleRobot();
    robot.addOpModeFactory(
        () ->
            new MockOpMode(
                constructedCount1,
                disabledPeriodicCount1,
                startCount1,
                periodicCount1,
                endCount1,
                closeCount1),
        RobotMode.TELEOPERATED,
        "OpMode1");
    robot.addOpModeFactory(
        () ->
            new MockOpMode(
                constructedCount2,
                disabledPeriodicCount2,
                startCount2,
                periodicCount2,
                endCount2,
                closeCount2),
        RobotMode.TELEOPERATED,
        "OpMode2");
    robot.publishOpModes();

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();
    SimHooks.waitForProgramStart();

    // 1. Select OpMode1 and enable
    DriverStationSim.setRobotMode(RobotMode.TELEOPERATED);
    DriverStationSim.setOpMode(makeOpModeId(RobotMode.TELEOPERATED, "OpMode1"));
    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(2 * kPeriod);
    assertEquals(1, constructedCount1.get());
    assertEquals(1, startCount1.get());
    assertEquals(1, periodicCount1.get());

    // 2. Change to OpMode2 while enabled
    DriverStationSim.setOpMode(makeOpModeId(RobotMode.TELEOPERATED, "OpMode2"));
    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(kPeriod);
    // OpMode1 should be ended and closed
    assertEquals(1, endCount1.get());
    assertEquals(1, closeCount1.get());

    // DS transitions to disabled on opmode switch, so enable again to start OpMode2
    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(2 * kPeriod);
    assertEquals(1, constructedCount2.get());
    assertEquals(1, startCount2.get());
    assertEquals(1, periodicCount2.get());

    robot.endCompetition();
    robotThread.join();
    robot.close();
  }

  @Test
  void testLifecycleOpModeChangeWhileDisabled() throws InterruptedException {
    AtomicInteger constructedCount1 = new AtomicInteger(0);
    AtomicInteger disabledPeriodicCount1 = new AtomicInteger(0);
    AtomicInteger startCount1 = new AtomicInteger(0);
    AtomicInteger periodicCount1 = new AtomicInteger(0);
    AtomicInteger endCount1 = new AtomicInteger(0);
    AtomicInteger closeCount1 = new AtomicInteger(0);

    AtomicInteger constructedCount2 = new AtomicInteger(0);
    AtomicInteger disabledPeriodicCount2 = new AtomicInteger(0);
    AtomicInteger startCount2 = new AtomicInteger(0);
    AtomicInteger periodicCount2 = new AtomicInteger(0);
    AtomicInteger endCount2 = new AtomicInteger(0);
    AtomicInteger closeCount2 = new AtomicInteger(0);

    LifecycleRobot robot = new LifecycleRobot();
    robot.addOpModeFactory(
        () ->
            new MockOpMode(
                constructedCount1,
                disabledPeriodicCount1,
                startCount1,
                periodicCount1,
                endCount1,
                closeCount1),
        RobotMode.TELEOPERATED,
        "OpMode1");
    robot.addOpModeFactory(
        () ->
            new MockOpMode(
                constructedCount2,
                disabledPeriodicCount2,
                startCount2,
                periodicCount2,
                endCount2,
                closeCount2),
        RobotMode.TELEOPERATED,
        "OpMode2");
    robot.publishOpModes();

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();
    SimHooks.waitForProgramStart();

    // 1. Select OpMode1 while disabled
    DriverStationSim.setRobotMode(RobotMode.TELEOPERATED);
    DriverStationSim.setOpMode(makeOpModeId(RobotMode.TELEOPERATED, "OpMode1"));
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(2 * kPeriod);
    assertEquals(1, constructedCount1.get());
    assertEquals(2, disabledPeriodicCount1.get());

    // 2. Change to OpMode2 while disabled
    DriverStationSim.setOpMode(makeOpModeId(RobotMode.TELEOPERATED, "OpMode2"));
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(2 * kPeriod);
    // OpMode1 should be closed, but NOT ended (since it never started)
    assertEquals(1, closeCount1.get());
    assertEquals(0, endCount1.get());
    // OpMode2 should be selected
    assertEquals(1, constructedCount2.get());
    assertEquals(1, disabledPeriodicCount2.get());

    robot.endCompetition();
    robotThread.join();
    robot.close();
  }
}
