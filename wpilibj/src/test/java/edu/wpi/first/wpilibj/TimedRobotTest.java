// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;

import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import java.util.ConcurrentModificationException;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;

class TimedRobotTest {
  static final double kPeriod = 0.02;

  static class MockRobot extends TimedRobot {
    public final AtomicInteger m_robotInitCount = new AtomicInteger(0);
    public final AtomicInteger m_simulationInitCount = new AtomicInteger(0);
    public final AtomicInteger m_disabledInitCount = new AtomicInteger(0);
    public final AtomicInteger m_autonomousInitCount = new AtomicInteger(0);
    public final AtomicInteger m_teleopInitCount = new AtomicInteger(0);
    public final AtomicInteger m_testInitCount = new AtomicInteger(0);

    public final AtomicInteger m_robotPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_simulationPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_disabledPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_autonomousPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_teleopPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_testPeriodicCount = new AtomicInteger(0);

    public final AtomicInteger m_disabledExitCount = new AtomicInteger(0);
    public final AtomicInteger m_autonomousExitCount = new AtomicInteger(0);
    public final AtomicInteger m_teleopExitCount = new AtomicInteger(0);
    public final AtomicInteger m_testExitCount = new AtomicInteger(0);

    MockRobot() {
      super(kPeriod);

      m_robotInitCount.addAndGet(1);
    }

    @Override
    public void simulationInit() {
      m_simulationInitCount.addAndGet(1);
    }

    @Override
    public void disabledInit() {
      m_disabledInitCount.addAndGet(1);
    }

    @Override
    public void autonomousInit() {
      m_autonomousInitCount.addAndGet(1);
    }

    @Override
    public void teleopInit() {
      m_teleopInitCount.addAndGet(1);
    }

    @Override
    public void testInit() {
      m_testInitCount.addAndGet(1);
    }

    @Override
    public void robotPeriodic() {
      m_robotPeriodicCount.addAndGet(1);
    }

    @Override
    public void simulationPeriodic() {
      m_simulationPeriodicCount.addAndGet(1);
    }

    @Override
    public void disabledPeriodic() {
      m_disabledPeriodicCount.addAndGet(1);
    }

    @Override
    public void autonomousPeriodic() {
      m_autonomousPeriodicCount.addAndGet(1);
    }

    @Override
    public void teleopPeriodic() {
      m_teleopPeriodicCount.addAndGet(1);
    }

    @Override
    public void testPeriodic() {
      m_testPeriodicCount.addAndGet(1);
    }

    @Override
    public void disabledExit() {
      m_disabledExitCount.addAndGet(1);
    }

    @Override
    public void autonomousExit() {
      m_autonomousExitCount.addAndGet(1);
    }

    @Override
    public void teleopExit() {
      m_teleopExitCount.addAndGet(1);
    }

    @Override
    public void testExit() {
      m_testExitCount.addAndGet(1);
    }
  }

  @BeforeEach
  void setup() {
    SimHooks.pauseTiming();
    DriverStationSim.resetData();
  }

  @AfterEach
  void cleanup() {
    SimHooks.resumeTiming();
  }

  @Test
  @ResourceLock("timing")
  void disabledModeTest() {
    MockRobot robot = new MockRobot();

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();

    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(0.0); // Wait for Notifiers

    assertEquals(1, robot.m_robotInitCount.get());
    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_testInitCount.get());

    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(0, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_testPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    SimHooks.stepTiming(kPeriod);

    assertEquals(1, robot.m_robotInitCount.get());
    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_testInitCount.get());

    assertEquals(1, robot.m_robotPeriodicCount.get());
    assertEquals(1, robot.m_simulationPeriodicCount.get());
    assertEquals(1, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_testPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    SimHooks.stepTiming(kPeriod);

    assertEquals(1, robot.m_robotInitCount.get());
    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_testInitCount.get());

    assertEquals(2, robot.m_robotPeriodicCount.get());
    assertEquals(2, robot.m_simulationPeriodicCount.get());
    assertEquals(2, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_testPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

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
  void autonomousModeTest() {
    MockRobot robot = new MockRobot();

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();

    DriverStationSim.setEnabled(true);
    DriverStationSim.setAutonomous(true);
    DriverStationSim.setTest(false);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(0.0); // Wait for Notifiers

    assertEquals(1, robot.m_robotInitCount.get());
    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_testInitCount.get());

    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(0, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_testPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    SimHooks.stepTiming(kPeriod);

    assertEquals(1, robot.m_robotInitCount.get());
    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_testInitCount.get());

    assertEquals(1, robot.m_robotPeriodicCount.get());
    assertEquals(1, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(1, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_testPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    SimHooks.stepTiming(kPeriod);

    assertEquals(1, robot.m_robotInitCount.get());
    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_testInitCount.get());

    assertEquals(2, robot.m_robotPeriodicCount.get());
    assertEquals(2, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(2, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_testPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

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
  void teleopModeTest() {
    MockRobot robot = new MockRobot();

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();

    DriverStationSim.setEnabled(true);
    DriverStationSim.setAutonomous(false);
    DriverStationSim.setTest(false);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(0.0); // Wait for Notifiers

    assertEquals(1, robot.m_robotInitCount.get());
    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_testInitCount.get());

    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(0, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_testPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    SimHooks.stepTiming(kPeriod);

    assertEquals(1, robot.m_robotInitCount.get());
    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(1, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_testInitCount.get());

    assertEquals(1, robot.m_robotPeriodicCount.get());
    assertEquals(1, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(1, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_testPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    SimHooks.stepTiming(kPeriod);

    assertEquals(1, robot.m_robotInitCount.get());
    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(1, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_testInitCount.get());

    assertEquals(2, robot.m_robotPeriodicCount.get());
    assertEquals(2, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(2, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_testPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    robot.endCompetition();
    try {
      robotThread.interrupt();
      robotThread.join();
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    robot.close();
  }

  @ValueSource(booleans = {true, false})
  @ParameterizedTest
  @ResourceLock("timing")
  void testModeTest(boolean isLW) {
    MockRobot robot = new MockRobot();
    robot.enableLiveWindowInTest(isLW);

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();

    DriverStationSim.setEnabled(true);
    DriverStationSim.setAutonomous(false);
    DriverStationSim.setTest(true);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(0.0); // Wait for Notifiers

    assertEquals(1, robot.m_robotInitCount.get());
    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_testInitCount.get());
    assertFalse(LiveWindow.isEnabled());

    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(0, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_testPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    SimHooks.stepTiming(kPeriod);

    assertEquals(1, robot.m_robotInitCount.get());
    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(1, robot.m_testInitCount.get());

    assertEquals(1, robot.m_robotPeriodicCount.get());
    assertEquals(1, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(1, robot.m_testPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    SimHooks.stepTiming(kPeriod);

    assertEquals(1, robot.m_robotInitCount.get());
    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(1, robot.m_testInitCount.get());
    assertEquals(isLW, LiveWindow.isEnabled());

    assertThrows(ConcurrentModificationException.class, () -> robot.enableLiveWindowInTest(isLW));

    assertEquals(2, robot.m_robotPeriodicCount.get());
    assertEquals(2, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(2, robot.m_testPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    DriverStationSim.setEnabled(false);
    DriverStationSim.setAutonomous(false);
    DriverStationSim.setTest(false);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(0.02);

    assertEquals(1, robot.m_robotInitCount.get());
    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(1, robot.m_testInitCount.get());
    assertFalse(LiveWindow.isEnabled());

    assertEquals(3, robot.m_robotPeriodicCount.get());
    assertEquals(3, robot.m_simulationPeriodicCount.get());
    assertEquals(1, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(2, robot.m_testPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(1, robot.m_testExitCount.get());

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
  void modeChangeTest() {
    MockRobot robot = new MockRobot();

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();

    // Start in disabled
    DriverStationSim.setEnabled(false);
    DriverStationSim.setAutonomous(false);
    DriverStationSim.setTest(false);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(0.0); // Wait for Notifiers

    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_testInitCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    SimHooks.stepTiming(kPeriod);

    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_testInitCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    // Transition to autonomous
    DriverStationSim.setEnabled(true);
    DriverStationSim.setAutonomous(true);
    DriverStationSim.setTest(false);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(kPeriod);

    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_testInitCount.get());

    assertEquals(1, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    // Transition to teleop
    DriverStationSim.setEnabled(true);
    DriverStationSim.setAutonomous(false);
    DriverStationSim.setTest(false);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(kPeriod);

    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_autonomousInitCount.get());
    assertEquals(1, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_testInitCount.get());

    assertEquals(1, robot.m_disabledExitCount.get());
    assertEquals(1, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    // Transition to test
    DriverStationSim.setEnabled(true);
    DriverStationSim.setAutonomous(false);
    DriverStationSim.setTest(true);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(kPeriod);

    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_autonomousInitCount.get());
    assertEquals(1, robot.m_teleopInitCount.get());
    assertEquals(1, robot.m_testInitCount.get());

    assertEquals(1, robot.m_disabledExitCount.get());
    assertEquals(1, robot.m_autonomousExitCount.get());
    assertEquals(1, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_testExitCount.get());

    // Transition to disabled
    DriverStationSim.setEnabled(false);
    DriverStationSim.setAutonomous(false);
    DriverStationSim.setTest(false);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(kPeriod);

    assertEquals(2, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_autonomousInitCount.get());
    assertEquals(1, robot.m_teleopInitCount.get());
    assertEquals(1, robot.m_testInitCount.get());

    assertEquals(1, robot.m_disabledExitCount.get());
    assertEquals(1, robot.m_autonomousExitCount.get());
    assertEquals(1, robot.m_teleopExitCount.get());
    assertEquals(1, robot.m_testExitCount.get());

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
  void addPeriodicTest() {
    MockRobot robot = new MockRobot();

    final AtomicInteger callbackCount = new AtomicInteger(0);
    robot.addPeriodic(() -> callbackCount.addAndGet(1), kPeriod / 2.0);

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();

    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(0.0); // Wait for Notifiers

    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, callbackCount.get());

    SimHooks.stepTiming(kPeriod / 2.0);

    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(1, callbackCount.get());

    SimHooks.stepTiming(kPeriod / 2.0);

    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_disabledPeriodicCount.get());
    assertEquals(2, callbackCount.get());

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
  void addPeriodicWithOffsetTest() {
    MockRobot robot = new MockRobot();

    final AtomicInteger callbackCount = new AtomicInteger(0);
    robot.addPeriodic(() -> callbackCount.addAndGet(1), kPeriod / 2.0, kPeriod / 4.0);

    // Expirations in this test (ms)
    //
    // Let p be period in ms.
    //
    // Robot | Callback
    // ================
    //     p |    0.75p
    //    2p |    1.25p

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();

    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();
    SimHooks.stepTiming(0.0); // Wait for Notifiers

    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, callbackCount.get());

    SimHooks.stepTiming(kPeriod * 3.0 / 8.0);

    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, callbackCount.get());

    SimHooks.stepTiming(kPeriod * 3.0 / 8.0);

    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(1, callbackCount.get());

    SimHooks.stepTiming(kPeriod / 4.0);

    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_disabledPeriodicCount.get());
    assertEquals(1, callbackCount.get());

    SimHooks.stepTiming(kPeriod / 4.0);

    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_disabledPeriodicCount.get());
    assertEquals(2, callbackCount.get());

    robot.endCompetition();
    try {
      robotThread.interrupt();
      robotThread.join();
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    robot.close();
  }
}
