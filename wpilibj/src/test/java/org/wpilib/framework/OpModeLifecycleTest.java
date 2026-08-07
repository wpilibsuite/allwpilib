// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.framework;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Set;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.wpilib.driverstation.RobotState;
import org.wpilib.hardware.hal.OpModeOption;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.internal.PeriodicPriorityQueue;
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

  // OpMode whose getCallbacks() returns a single callback with no enabled check,
  // used to verify that getCallbacks() callbacks are registered immediately and
  // run even while disabled.
  static class CallbackOpMode implements OpMode {
    private final AtomicInteger m_callbackCount;

    CallbackOpMode(AtomicInteger callbackCount) {
      m_callbackCount = callbackCount;
    }

    @Override
    public Set<PeriodicPriorityQueue.Callback> getCallbacks() {
      return Set.of(
          new PeriodicPriorityQueue.Callback(m_callbackCount::incrementAndGet, 0, kPeriod));
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
    RobotState.clearOpModes();
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
    assertEquals(0, periodicCount.get());

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

    // 2. Switch to OpMode2 while enabled. Selecting a different opmode while
    // enabled disables the robot first, so the DS sends disabled + new opmode.
    DriverStationSim.setOpMode(makeOpModeId(RobotMode.TELEOPERATED, "OpMode2"));
    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(kPeriod);
    // OpMode1 should be ended and closed
    assertEquals(1, endCount1.get());
    assertEquals(1, closeCount1.get());
    // OpMode2 should be constructed exactly once and persist while disabled
    assertEquals(1, constructedCount2.get());
    assertEquals(0, startCount2.get());

    // 3. Re-enable. The same OpMode2 instance is started; it is not reconstructed.
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
    SimHooks.stepTiming(kPeriod);
    assertEquals(1, constructedCount1.get());
    assertEquals(1, disabledPeriodicCount1.get());

    // 2. Change to OpMode2 while disabled
    DriverStationSim.setOpMode(makeOpModeId(RobotMode.TELEOPERATED, "OpMode2"));
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(kPeriod);
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

  @Test
  void testGetCallbacksRunImmediatelyWhileDisabled() throws InterruptedException {
    AtomicInteger callbackCount = new AtomicInteger(0);
    LifecycleRobot robot = new LifecycleRobot();
    robot.addOpModeFactory(
        () -> new CallbackOpMode(callbackCount), RobotMode.TELEOPERATED, "CallbackOpMode");
    robot.publishOpModes();

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();
    SimHooks.waitForProgramStart();

    // Select the opmode while disabled. getCallbacks() callbacks are registered
    // immediately on construction and run even while the robot is disabled.
    DriverStationSim.setRobotMode(RobotMode.TELEOPERATED);
    DriverStationSim.setOpMode(makeOpModeId(RobotMode.TELEOPERATED, "CallbackOpMode"));
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(5 * kPeriod);
    assertTrue(callbackCount.get() >= 1);

    // Deselecting the opmode tears it down and removes its callbacks, so the
    // callback must stop running.
    DriverStationSim.setOpMode(0);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(5 * kPeriod); // let teardown settle
    int countAfterTeardown = callbackCount.get();
    SimHooks.stepTiming(5 * kPeriod);
    assertEquals(countAfterTeardown, callbackCount.get());

    robot.endCompetition();
    robotThread.join();
    robot.close();
  }

  @Test
  void testLifecycleInitialEnabledState() throws InterruptedException {
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

    // The very first DS packet is fully enabled and has an opmode selected
    DriverStationSim.setDsAttached(true);
    DriverStationSim.setRobotMode(RobotMode.TELEOPERATED);
    DriverStationSim.setOpMode(makeOpModeId(RobotMode.TELEOPERATED, "TestOpMode"));
    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(2 * kPeriod);

    // Should construct, call disabledPeriodic once (since it's a new opmode), then start and
    // periodic
    assertEquals(1, constructedCount.get());
    assertEquals(1, disabledPeriodicCount.get());
    assertEquals(1, startCount.get());
    assertEquals(1, periodicCount.get());

    robot.endCompetition();
    robotThread.join();
    robot.close();
  }

  @Test
  void testLifecycleReconstructionOnDisable() throws InterruptedException {
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

    // 1. Enable
    DriverStationSim.setRobotMode(RobotMode.TELEOPERATED);
    DriverStationSim.setOpMode(makeOpModeId(RobotMode.TELEOPERATED, "TestOpMode"));
    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(2 * kPeriod);

    assertEquals(1, constructedCount.get());
    assertEquals(1, startCount.get());

    // 2. Disable
    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(2 * kPeriod);

    // Old instance ended and closed
    assertEquals(1, endCount.get());
    assertEquals(1, closeCount.get());

    // New instance should be constructed and disabledPeriodic called
    assertEquals(2, constructedCount.get());
    assertTrue(disabledPeriodicCount.get() >= 1);
    assertEquals(1, startCount.get()); // new instance not started yet

    // 3. Re-enable
    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(2 * kPeriod);

    assertEquals(2, constructedCount.get());
    assertEquals(2, startCount.get());
    assertEquals(1, endCount.get());

    robot.endCompetition();
    robotThread.join();
    robot.close();
  }

  @Test
  void testLifecycleDeselectOpMode() throws InterruptedException {
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

    DriverStationSim.setRobotMode(RobotMode.TELEOPERATED);
    DriverStationSim.setOpMode(makeOpModeId(RobotMode.TELEOPERATED, "TestOpMode"));
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(kPeriod);

    assertEquals(1, constructedCount.get());

    // Deselect opmode
    DriverStationSim.setOpMode(0);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(kPeriod);

    assertEquals(1, closeCount.get());
    assertEquals(1, constructedCount.get()); // no new instance constructed

    robot.endCompetition();
    robotThread.join();
    robot.close();
  }

  @Test
  void testLifecycleDsDisconnect() throws InterruptedException {
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

    DriverStationSim.setEnabled(true);
    DriverStationSim.setRobotMode(RobotMode.TELEOPERATED);
    DriverStationSim.setOpMode(makeOpModeId(RobotMode.TELEOPERATED, "TestOpMode"));
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(2 * kPeriod);

    assertEquals(1, constructedCount.get());
    assertEquals(1, startCount.get());

    // DS Disconnect
    DriverStationSim.setDsAttached(false);
    // DriverStationSim.notifyNewData(); // DON'T DO THIS
    SimHooks.stepTiming(2 * kPeriod);

    assertEquals(1, endCount.get());
    assertEquals(1, closeCount.get());
    assertEquals(1, constructedCount.get()); // no new instance constructed

    robot.endCompetition();
    robotThread.join();
    robot.close();
  }
}
