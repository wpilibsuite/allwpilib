// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.framework;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.simulation.DriverStationSim;
import org.wpilib.simulation.SimHooks;
import org.wpilib.telemetry.Telemetry;
import org.wpilib.telemetry.TelemetryRegistry;
import org.wpilib.tunable.Tunable;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.tunable.Tunables;
import org.wpilib.units.Units;
import org.wpilib.units.measure.Distance;
import org.wpilib.util.AlertDataJNI;
import org.wpilib.util.WPIUtilJNI;

class TimedRobotTest {
  static final double PERIOD = 0.02;

  @SuppressWarnings("PMD.PublicFieldNamingConvention")
  static class MockRobot extends TimedRobot {
    public final AtomicInteger m_simulationInitCount = new AtomicInteger(0);
    public final AtomicInteger m_disabledInitCount = new AtomicInteger(0);
    public final AtomicInteger m_autonomousInitCount = new AtomicInteger(0);
    public final AtomicInteger m_teleopInitCount = new AtomicInteger(0);
    public final AtomicInteger m_utilityInitCount = new AtomicInteger(0);

    public final AtomicInteger m_robotPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_simulationPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_disabledPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_autonomousPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_teleopPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_utilityPeriodicCount = new AtomicInteger(0);

    public final AtomicInteger m_disabledExitCount = new AtomicInteger(0);
    public final AtomicInteger m_autonomousExitCount = new AtomicInteger(0);
    public final AtomicInteger m_teleopExitCount = new AtomicInteger(0);
    public final AtomicInteger m_utilityExitCount = new AtomicInteger(0);

    MockRobot() {
      super(PERIOD);
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
    public void utilityInit() {
      m_utilityInitCount.addAndGet(1);
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
    public void utilityPeriodic() {
      m_utilityPeriodicCount.addAndGet(1);
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
    public void utilityExit() {
      m_utilityExitCount.addAndGet(1);
    }
  }

  @BeforeEach
  void setup() {
    SimHooks.pauseTiming();
    SimHooks.setProgramStarted(false);
    DriverStationSim.resetData();
  }

  @AfterEach
  void cleanup() {
    SimHooks.resumeTiming();
  }

  @Test
  void robotNameTest() {
    // Simulated stack trace from a robot crash
    StackTraceElement[] elements = {
      new StackTraceElement("org.wpilib.framework.TimedRobot", "<init>", null, 1),
      new StackTraceElement("org.wpilib.framework.TimedRobotTest$MockRobot", "<init>", null, 1),
      new StackTraceElement(
          "jdk.internal.reflect.DirectConstructorHandleAccessor", "newInstance", null, 1),
      new StackTraceElement("java.lang.reflect.Constructor", "newInstanceWithCaller", null, 1),
      new StackTraceElement("java.lang.reflect.Constructor", "newInstance", null, 1),
      new StackTraceElement("org.wpilib.util.ConstructorMatch", "newInstance", null, 1),
      new StackTraceElement("org.wpilib.framework.RobotBase", "constructRobot", null, 1),
      new StackTraceElement("org.wpilib.framework.RobotBase", "runRobot", null, 1),
      new StackTraceElement("org.wpilib.framework.RobotBase", "lambda$startRobot$0", null, 1),
      new StackTraceElement("java.lang.Thread", "run", null, 1)
    };
    assertEquals("org.wpilib.framework.TimedRobotTest$MockRobot", MockRobot.getRobotName(elements));
  }

  @Test
  @ResourceLock("timing")
  void constructorPublishesProgramStartTime() {
    try (var sub =
            NetworkTableInstance.getDefault()
                .getIntegerTopic("/Robot/ProgramStartTime")
                .subscribe(-1);
        var robot = new MockRobot()) {
      assertEquals(WPIUtilJNI.getProgramStartTime(), sub.get(-1));
    }
  }

  @Test
  @ResourceLock("timing")
  @SuppressWarnings({"rawtypes", "unchecked"})
  void constructorRegistersMeasureTelemetryAndTunableHandlers() {
    TelemetryRegistry.reset();
    TunableRegistry.reset();

    try (var robot = new MockRobot()) {
      var inst = NetworkTableInstance.getDefault();

      Telemetry.log("telemetryDistance", Units.Meter.of(2.5));

      assertEquals(
          2.5, inst.getTopic("/Telemetry/telemetryDistance").getGenericEntry().getDouble(0.0));
      assertEquals("\"m\"", inst.getTopic("/Telemetry/telemetryDistance").getProperty("unit"));

      Telemetry.log("telemetryDistance", Units.Feet.of(1.0));

      assertEquals(
          Units.Feet.of(1.0).in(Units.Meters),
          inst.getTopic("/Telemetry/telemetryDistance").getGenericEntry().getDouble(0.0),
          1e-9);
      assertEquals("\"m\"", inst.getTopic("/Telemetry/telemetryDistance").getProperty("unit"));

      Tunable<Distance> tunableDistance = Tunable.create(Units.Meter.of(0.0));
      Tunables.publish("tunableDistance", tunableDistance);

      assertFalse(tunableDistance.hasChanged());
      assertEquals(0.0, inst.getTopic("/Tunables/tunableDistance").getGenericEntry().getDouble(-1));
      assertEquals("\"m\"", inst.getTopic("/Tunables/tunableDistance").getProperty("unit"));

      inst.getTopic("/Tunables/tunableDistance").getGenericEntry().setDouble(3.0);
      inst.flush();
      TunableRegistry.update();

      assertEquals(3.0, tunableDistance.get().in(Units.Meter));
      assertFalse(tunableDistance.hasChanged());

      assertEquals(3.0, tunableDistance.mutate().in(Units.Meter));
      assertTrue(tunableDistance.hasChanged());
      TunableRegistry.update();
      assertFalse(tunableDistance.hasChanged());

      tunableDistance.set(Units.Feet.of(1.0));
      assertTrue(tunableDistance.hasChanged());
      TunableRegistry.update();

      assertEquals(
          Units.Feet.of(1.0).in(Units.Meters),
          inst.getTopic("/Tunables/tunableDistance").getGenericEntry().getDouble(-1),
          1e-9);
      assertEquals("\"m\"", inst.getTopic("/Tunables/tunableDistance").getProperty("unit"));
      assertEquals(1.0, tunableDistance.get().in(Units.Feet), 1e-9);
      assertFalse(tunableDistance.hasChanged());

      inst.getTopic("/Tunables/tunableDistance").getGenericEntry().setDouble(2.0);
      inst.flush();
      TunableRegistry.update();

      assertEquals(2.0, tunableDistance.get().in(Units.Meters));
      assertFalse(tunableDistance.hasChanged());

      Tunable rawTunableDistance = tunableDistance;
      assertThrows(
          IllegalArgumentException.class, () -> rawTunableDistance.set(Units.Volts.of(1.0)));

      Tunable<Distance> feetTunableDistance = Tunable.create(Units.Feet.of(6.0));
      Tunables.publish("feetTunableDistance", feetTunableDistance);

      assertEquals(
          Units.Feet.of(6.0).in(Units.Meters),
          inst.getTopic("/Tunables/feetTunableDistance").getGenericEntry().getDouble(-1),
          1e-9);
      assertEquals("\"m\"", inst.getTopic("/Tunables/feetTunableDistance").getProperty("unit"));

      inst.getTopic("/Tunables/feetTunableDistance").getGenericEntry().setDouble(2.0);
      inst.flush();
      TunableRegistry.update();

      assertEquals(2.0, feetTunableDistance.get().in(Units.Meters));
    } finally {
      TelemetryRegistry.reset();
      TunableRegistry.reset();
    }
  }

  @Test
  @ResourceLock("timing")
  void constructorMapsWarningsToAlerts() {
    TelemetryRegistry.reset();
    TunableRegistry.reset();
    AlertDataJNI.resetData();

    try (var robot = new MockRobot()) {
      TelemetryRegistry.reportWarning("/bad", "telemetry test warning");
      TunableRegistry.reportWarning("tunable test warning");

      AlertDataJNI.AlertInfo[] alerts = AlertDataJNI.getAlerts();
      assertEquals(2, alerts.length);

      boolean sawTelemetry = false;
      boolean sawTunable = false;
      for (AlertDataJNI.AlertInfo alert : alerts) {
        assertNotEquals(0, alert.activeStartTime);
        if ("Telemetry".equals(alert.group) && alert.text.contains("telemetry test warning")) {
          sawTelemetry = true;
        }
        if ("Tunables".equals(alert.group) && alert.text.contains("tunable test warning")) {
          sawTunable = true;
        }
      }
      assertTrue(sawTelemetry);
      assertTrue(sawTunable);
    }

    assertEquals(0, AlertDataJNI.getAlerts().length);
    AlertDataJNI.resetData();
  }

  @Test
  @ResourceLock("timing")
  void disabledModeTest() {
    MockRobot robot = new MockRobot();

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();
    SimHooks.waitForProgramStart();

    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();

    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_utilityInitCount.get());

    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(0, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_utilityInitCount.get());

    assertEquals(1, robot.m_robotPeriodicCount.get());
    assertEquals(1, robot.m_simulationPeriodicCount.get());
    assertEquals(1, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_utilityInitCount.get());

    assertEquals(2, robot.m_robotPeriodicCount.get());
    assertEquals(2, robot.m_simulationPeriodicCount.get());
    assertEquals(2, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

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
    SimHooks.waitForProgramStart();

    DriverStationSim.setEnabled(true);
    DriverStationSim.setRobotMode(RobotMode.AUTONOMOUS);
    DriverStationSim.notifyNewData();

    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_utilityInitCount.get());

    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(0, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_utilityInitCount.get());

    assertEquals(1, robot.m_robotPeriodicCount.get());
    assertEquals(1, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(1, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_utilityInitCount.get());

    assertEquals(2, robot.m_robotPeriodicCount.get());
    assertEquals(2, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(2, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

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
    SimHooks.waitForProgramStart();

    DriverStationSim.setEnabled(true);
    DriverStationSim.setRobotMode(RobotMode.TELEOPERATED);
    DriverStationSim.notifyNewData();

    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_utilityInitCount.get());

    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(0, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(1, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_utilityInitCount.get());

    assertEquals(1, robot.m_robotPeriodicCount.get());
    assertEquals(1, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(1, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(1, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_utilityInitCount.get());

    assertEquals(2, robot.m_robotPeriodicCount.get());
    assertEquals(2, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(2, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

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
  void utilityModeTest() {
    MockRobot robot = new MockRobot();

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();
    SimHooks.waitForProgramStart();

    DriverStationSim.setEnabled(true);
    DriverStationSim.setRobotMode(RobotMode.UTILITY);
    DriverStationSim.notifyNewData();

    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_utilityInitCount.get());

    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(0, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(1, robot.m_utilityInitCount.get());

    assertEquals(1, robot.m_robotPeriodicCount.get());
    assertEquals(1, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(1, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(1, robot.m_utilityInitCount.get());

    assertEquals(2, robot.m_robotPeriodicCount.get());
    assertEquals(2, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(2, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(0.02);

    assertEquals(1, robot.m_simulationInitCount.get());
    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(1, robot.m_utilityInitCount.get());

    assertEquals(3, robot.m_robotPeriodicCount.get());
    assertEquals(3, robot.m_simulationPeriodicCount.get());
    assertEquals(1, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(2, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(1, robot.m_utilityExitCount.get());

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
    SimHooks.waitForProgramStart();

    // Start in disabled
    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();

    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_utilityInitCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_utilityInitCount.get());

    assertEquals(0, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

    // Transition to autonomous
    DriverStationSim.setEnabled(true);
    DriverStationSim.setRobotMode(RobotMode.AUTONOMOUS);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_autonomousInitCount.get());
    assertEquals(0, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_utilityInitCount.get());

    assertEquals(1, robot.m_disabledExitCount.get());
    assertEquals(0, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

    // Transition to teleop
    DriverStationSim.setEnabled(true);
    DriverStationSim.setRobotMode(RobotMode.TELEOPERATED);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_autonomousInitCount.get());
    assertEquals(1, robot.m_teleopInitCount.get());
    assertEquals(0, robot.m_utilityInitCount.get());

    assertEquals(1, robot.m_disabledExitCount.get());
    assertEquals(1, robot.m_autonomousExitCount.get());
    assertEquals(0, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

    // Transition to utility
    DriverStationSim.setEnabled(true);
    DriverStationSim.setRobotMode(RobotMode.UTILITY);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_autonomousInitCount.get());
    assertEquals(1, robot.m_teleopInitCount.get());
    assertEquals(1, robot.m_utilityInitCount.get());

    assertEquals(1, robot.m_disabledExitCount.get());
    assertEquals(1, robot.m_autonomousExitCount.get());
    assertEquals(1, robot.m_teleopExitCount.get());
    assertEquals(0, robot.m_utilityExitCount.get());

    // Transition to disabled
    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(PERIOD);

    assertEquals(2, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_autonomousInitCount.get());
    assertEquals(1, robot.m_teleopInitCount.get());
    assertEquals(1, robot.m_utilityInitCount.get());

    assertEquals(1, robot.m_disabledExitCount.get());
    assertEquals(1, robot.m_autonomousExitCount.get());
    assertEquals(1, robot.m_teleopExitCount.get());
    assertEquals(1, robot.m_utilityExitCount.get());

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
    robot.addPeriodic(() -> callbackCount.addAndGet(1), PERIOD / 2.0);

    Thread robotThread = new Thread(robot::startCompetition);
    robotThread.start();
    SimHooks.waitForProgramStart();

    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();

    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, callbackCount.get());

    SimHooks.stepTiming(PERIOD / 2.0);

    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(1, callbackCount.get());

    SimHooks.stepTiming(PERIOD / 2.0);

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
    robot.addPeriodic(() -> callbackCount.addAndGet(1), PERIOD / 2.0, PERIOD / 4.0);

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
    SimHooks.waitForProgramStart();

    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();

    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, callbackCount.get());

    SimHooks.stepTiming(PERIOD * 3.0 / 8.0);

    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, callbackCount.get());

    SimHooks.stepTiming(PERIOD * 3.0 / 8.0);

    assertEquals(0, robot.m_disabledInitCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(1, callbackCount.get());

    SimHooks.stepTiming(PERIOD / 4.0);

    assertEquals(1, robot.m_disabledInitCount.get());
    assertEquals(1, robot.m_disabledPeriodicCount.get());
    assertEquals(1, callbackCount.get());

    SimHooks.stepTiming(PERIOD / 4.0);

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
