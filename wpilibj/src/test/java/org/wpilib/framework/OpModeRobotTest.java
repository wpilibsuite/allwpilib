// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.framework;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.fail;

import java.util.Arrays;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.wpilib.driverstation.internal.DriverStationBackend;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.opmode.OpMode;
import org.wpilib.simulation.DriverStationSim;
import org.wpilib.simulation.SimHooks;
import org.wpilib.util.Color;

@ResourceLock("timing")
class OpModeRobotTest {
  static final double kPeriod = 0.02;

  @SuppressWarnings("PMD.PublicFieldNamingConvention")
  public static class MockOpMode implements OpMode {
    public final AtomicInteger m_disabledPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_startCount = new AtomicInteger(0);
    public final AtomicInteger m_periodicCount = new AtomicInteger(0);
    public final AtomicInteger m_endCount = new AtomicInteger(0);
    public final AtomicInteger m_closeCount = new AtomicInteger(0);

    MockOpMode() {}

    @Override
    public void close() {
      m_closeCount.incrementAndGet();
    }

    @Override
    public void disabledPeriodic() {
      m_disabledPeriodicCount.incrementAndGet();
    }

    @Override
    public void start() {
      m_startCount.incrementAndGet();
    }

    @Override
    public void periodic() {
      m_periodicCount.incrementAndGet();
    }

    @Override
    public void end() {
      m_endCount.incrementAndGet();
    }
  }

  public static class OneArgOpMode implements OpMode {
    @SuppressWarnings("unused")
    OneArgOpMode(MockRobot robot) {}
  }

  @SuppressWarnings("PMD.PublicFieldNamingConvention")
  static class MockRobot extends OpModeRobot {
    public final AtomicInteger m_driverStationConnectedCount = new AtomicInteger(0);
    public final AtomicInteger m_nonePeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_robotPeriodicCount = new AtomicInteger(0);

    MockRobot() {
      super();
    }

    @Override
    public void driverStationConnected() {
      m_driverStationConnectedCount.incrementAndGet();
    }

    @Override
    public void nonePeriodic() {
      m_nonePeriodicCount.incrementAndGet();
    }

    @Override
    public void robotPeriodic() {
      m_robotPeriodicCount.incrementAndGet();
    }
  }

  @BeforeEach
  void setUp() {
    SimHooks.pauseTiming();
    SimHooks.setProgramStarted(false);
    DriverStationSim.resetData();
  }

  @AfterEach
  void tearDown() {
    DriverStationBackend.clearOpModes();
    SimHooks.resumeTiming();
  }

  @AfterEach
  @SuppressWarnings("PMD.AvoidAccessibilityAlteration")
  void resetUserProgramFlag() throws ReflectiveOperationException {
    var field = DriverStationBackend.class.getDeclaredField("m_userProgramStarted");
    field.setAccessible(true);
    field.set(null, false);
  }

  @Test
  void addOpMode() {
    class MyMockRobot extends MockRobot {
      MyMockRobot() {
        addOpModeFactory(
            () -> new MockOpMode(),
            RobotMode.AUTONOMOUS,
            "NoArgOpMode-Auto",
            "Group",
            "Description",
            Color.WHITE,
            Color.BLACK);
        addOpModeFactory(
            () -> new OneArgOpMode(this),
            RobotMode.UTILITY,
            "OneArgOpMode-Test",
            "Group",
            "Description",
            Color.WHITE,
            Color.BLACK);
        addOpModeFactory(() -> new MockOpMode(), RobotMode.TELEOPERATED, "NoArgOpMode");
        addOpModeFactory(() -> new OneArgOpMode(this), RobotMode.TELEOPERATED, "OneArgOpMode");
        publishOpModes();
      }
    }

    final MyMockRobot robot = new MyMockRobot();

    var options = Arrays.asList(DriverStationSim.getOpModeOptions());
    assertEquals(4, options.size());

    int[] indexes = {-1, -1, -1, -1};
    for (int i = 0; i < options.size(); i++) {
      String name = options.get(i).name;
      switch (name) {
        case "NoArgOpMode-Auto" -> indexes[0] = i;
        case "OneArgOpMode-Test" -> indexes[1] = i;
        case "NoArgOpMode" -> indexes[2] = i;
        case "OneArgOpMode" -> indexes[3] = i;
        default -> fail("Unexpected op mode: " + name + " at index " + i);
      }
    }

    int i = indexes[0];
    assertNotEquals(-1, i);
    assertEquals("Group", options.get(i).group);
    assertEquals("Description", options.get(i).description);
    assertEquals(0xffffff, options.get(i).textColor);
    assertEquals(0x000000, options.get(i).backgroundColor);

    i = indexes[1];
    assertNotEquals(-1, i);
    assertEquals("Group", options.get(i).group);
    assertEquals("Description", options.get(i).description);
    assertEquals(0xffffff, options.get(i).textColor);
    assertEquals(0x000000, options.get(i).backgroundColor);

    i = indexes[2];
    assertNotEquals(-1, i);
    assertEquals("", options.get(i).group);
    assertEquals("", options.get(i).description);
    assertEquals(-1, options.get(i).textColor);
    assertEquals(-1, options.get(i).backgroundColor);

    i = indexes[3];
    assertNotEquals(-1, i);
    assertEquals("", options.get(i).group);
    assertEquals("", options.get(i).description);
    assertEquals(-1, options.get(i).textColor);
    assertEquals(-1, options.get(i).backgroundColor);

    robot.close();
  }

  @Test
  void clearOpModes() {
    class MyMockRobot extends MockRobot {
      MyMockRobot() {
        addOpModeFactory(() -> new MockOpMode(), RobotMode.TELEOPERATED, "NoArgOpMode");
        addOpModeFactory(() -> new OneArgOpMode(this), RobotMode.TELEOPERATED, "OneArgOpMode");
        publishOpModes();
      }
    }

    MyMockRobot robot = new MyMockRobot();

    robot.clearOpModes();
    var options = DriverStationSim.getOpModeOptions();
    assertEquals(0, options.length);

    robot.close();
  }

  @Test
  void removeOpMode() {
    class MyMockRobot extends MockRobot {
      MyMockRobot() {
        addOpModeFactory(() -> new MockOpMode(), RobotMode.TELEOPERATED, "NoArgOpMode");
        addOpModeFactory(() -> new OneArgOpMode(this), RobotMode.TELEOPERATED, "OneArgOpMode");
        publishOpModes();
      }
    }

    MyMockRobot robot = new MyMockRobot();

    robot.removeOpMode(RobotMode.TELEOPERATED, "NoArgOpMode");
    robot.publishOpModes();
    var options = DriverStationSim.getOpModeOptions();
    assertEquals(1, options.length);
    assertEquals("OneArgOpMode", options[0].name);

    robot.close();
  }

  @Test
  void nonePeriodic() throws InterruptedException {
    class MyMockRobot extends MockRobot {
      MyMockRobot() {
        addOpModeFactory(() -> new MockOpMode(), RobotMode.TELEOPERATED, "NoArgOpMode");
        addOpModeFactory(() -> new OneArgOpMode(this), RobotMode.TELEOPERATED, "OneArgOpMode");
        publishOpModes();
      }
    }

    MyMockRobot robot = new MyMockRobot();

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();
    SimHooks.waitForProgramStart();

    // Time step to get periodic calls on 20 ms robot loop
    SimHooks.stepTiming(0.11); // 110ms
    assertEquals(5, robot.m_nonePeriodicCount.get());

    robot.endCompetition();
    robotThread.join();
    robot.close();
  }

  @Test
  void robotPeriodic() throws InterruptedException {
    class MyMockRobot extends MockRobot {
      MyMockRobot() {
        addOpModeFactory(() -> new MockOpMode(), RobotMode.TELEOPERATED, "TestOpMode");
        publishOpModes();
      }
    }

    MyMockRobot robot = new MyMockRobot();

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();
    SimHooks.waitForProgramStart();

    // RobotPeriodic should be called regardless of state
    assertEquals(0, robot.m_robotPeriodicCount.get());

    // Step timing to allow callbacks to execute
    SimHooks.stepTiming(kPeriod);
    assertEquals(1, robot.m_robotPeriodicCount.get());

    // Additional time steps should continue calling robotPeriodic
    SimHooks.stepTiming(kPeriod);
    assertEquals(2, robot.m_robotPeriodicCount.get());

    robot.endCompetition();
    robotThread.join();
    robot.close();
  }
}
