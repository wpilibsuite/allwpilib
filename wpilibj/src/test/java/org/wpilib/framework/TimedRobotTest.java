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
import java.util.concurrent.atomic.AtomicReference;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.simulation.AlertSim;
import org.wpilib.simulation.DriverStationSim;
import org.wpilib.simulation.SimHooks;
import org.wpilib.system.WPILibVersion;
import org.wpilib.telemetry.Telemetry;
import org.wpilib.telemetry.TelemetryRegistry;
import org.wpilib.tunable.Tunable;
import org.wpilib.tunable.TunableConfig;
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
    public final AtomicInteger m_simulationStartCount = new AtomicInteger(0);
    public final AtomicInteger m_disabledStartCount = new AtomicInteger(0);
    public final AtomicInteger m_autonomousStartCount = new AtomicInteger(0);
    public final AtomicInteger m_teleopStartCount = new AtomicInteger(0);
    public final AtomicInteger m_utilityStartCount = new AtomicInteger(0);

    public final AtomicInteger m_robotPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_simulationPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_disabledPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_autonomousPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_teleopPeriodicCount = new AtomicInteger(0);
    public final AtomicInteger m_utilityPeriodicCount = new AtomicInteger(0);

    public final AtomicInteger m_disabledEndCount = new AtomicInteger(0);
    public final AtomicInteger m_autonomousEndCount = new AtomicInteger(0);
    public final AtomicInteger m_teleopEndCount = new AtomicInteger(0);
    public final AtomicInteger m_utilityEndCount = new AtomicInteger(0);

    MockRobot() {
      super(PERIOD);
    }

    @Override
    public void simulationStart() {
      m_simulationStartCount.addAndGet(1);
    }

    @Override
    public void disabledStart() {
      m_disabledStartCount.addAndGet(1);
    }

    @Override
    public void autonomousStart() {
      m_autonomousStartCount.addAndGet(1);
    }

    @Override
    public void teleopStart() {
      m_teleopStartCount.addAndGet(1);
    }

    @Override
    public void utilityStart() {
      m_utilityStartCount.addAndGet(1);
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
    public void disabledEnd() {
      m_disabledEndCount.addAndGet(1);
    }

    @Override
    public void autonomousEnd() {
      m_autonomousEndCount.addAndGet(1);
    }

    @Override
    public void teleopEnd() {
      m_teleopEndCount.addAndGet(1);
    }

    @Override
    public void utilityEnd() {
      m_utilityEndCount.addAndGet(1);
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
  void constructorPublishesWPILibVersion() {
    var topic = NetworkTableInstance.getDefault().getStringTopic("/Robot/WPILibVersion");
    try (var robot = new MockRobot();
        var sub = topic.subscribe("")) {
      assertEquals(WPILibVersion.Version, sub.get());
    }
    assertFalse(topic.exists());
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
  void measureGetterSetterTunablesPublishAndTuneInBaseUnits() {
    TelemetryRegistry.reset();
    TunableRegistry.reset();
    var inst = NetworkTableInstance.getDefault();

    try (var robot = new MockRobot();
        var valueEntry = inst.getDoubleTopic("/Tunables/getterDistance/value").getEntry(-1);
        var tuneEntry = inst.getDoubleTopic("/Tunables/getterDistance/tune").getEntry(-1)) {
      AtomicReference<Distance> value = new AtomicReference<>(Units.Inches.of(5));
      AtomicInteger calls = new AtomicInteger();
      final Tunable<Distance> tunable =
          Tunables.publishValue(
              "getterDistance",
              value::get,
              value::set,
              Distance.class,
              new TunableConfig().withRobust(true).withOnTune(calls::incrementAndGet));

      assertEquals(0.127, valueEntry.get(), 1e-9);
      assertEquals("\"m\"", valueEntry.getTopic().getProperty("unit"));

      value.set(Units.Feet.of(2));
      TunableRegistry.update();
      assertEquals(0.6096, valueEntry.get(), 1e-9);
      assertEquals(value.get(), tunable.get());
      assertEquals(0, calls.get());

      tunable.set(Units.Feet.of(3));
      assertTrue(tunable.hasChanged());
      assertEquals(36, value.get().in(Units.Inches), 1e-9);
      TunableRegistry.update();
      assertEquals(0.9144, valueEntry.get(), 1e-9);
      assertFalse(tunable.hasChanged());
      assertEquals(0, calls.get());

      tuneEntry.set(1.0);
      inst.flush();
      TunableRegistry.update();

      assertEquals(1.0, value.get().in(Units.Meters), 1e-9);
      assertEquals(Units.Inches, value.get().unit());
      assertEquals(1.0, valueEntry.get(), 1e-9);
      assertEquals(1, calls.get());
      assertEquals(1, TunableRegistry.getTuneRevision(tunable));
      assertFalse(tunable.hasChanged());
    } finally {
      TelemetryRegistry.reset();
      TunableRegistry.reset();
    }
  }

  @Test
  @ResourceLock("timing")
  void measureGetterTunableSupportsNullSetter() {
    TelemetryRegistry.reset();
    TunableRegistry.reset();
    var inst = NetworkTableInstance.getDefault();

    try (var robot = new MockRobot();
        var entry = inst.getDoubleTopic("/Tunables/readOnlyDistance").getEntry(-1)) {
      AtomicReference<Distance> value = new AtomicReference<>(Units.Feet.of(2));
      final Tunable<Distance> tunable =
          Tunables.publishValue("readOnlyDistance", value::get, null, Distance.class);

      assertEquals(0.6096, entry.get(), 1e-9);
      assertEquals("\"m\"", entry.getTopic().getProperty("unit"));

      value.set(Units.Feet.of(3));
      TunableRegistry.update();
      assertEquals(0.9144, entry.get(), 1e-9);

      tunable.set(Units.Meters.of(1));
      assertEquals(3, value.get().in(Units.Feet), 1e-9);

      entry.set(2.0);
      inst.flush();
      TunableRegistry.update();
      assertEquals(3, value.get().in(Units.Feet), 1e-9);
      assertEquals(value.get(), tunable.get());
      assertEquals(0.9144, entry.get(), 1e-9);
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

      AlertSim.AlertInfo[] alerts = AlertSim.getActive();
      assertEquals(2, alerts.length);

      boolean sawTelemetry = false;
      boolean sawTunable = false;
      for (AlertSim.AlertInfo alert : alerts) {
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

    assertEquals(1, robot.m_simulationStartCount.get());
    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_autonomousStartCount.get());
    assertEquals(0, robot.m_teleopStartCount.get());
    assertEquals(0, robot.m_utilityStartCount.get());

    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(0, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationStartCount.get());
    assertEquals(1, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_autonomousStartCount.get());
    assertEquals(0, robot.m_teleopStartCount.get());
    assertEquals(0, robot.m_utilityStartCount.get());

    assertEquals(1, robot.m_robotPeriodicCount.get());
    assertEquals(1, robot.m_simulationPeriodicCount.get());
    assertEquals(1, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationStartCount.get());
    assertEquals(1, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_autonomousStartCount.get());
    assertEquals(0, robot.m_teleopStartCount.get());
    assertEquals(0, robot.m_utilityStartCount.get());

    assertEquals(2, robot.m_robotPeriodicCount.get());
    assertEquals(2, robot.m_simulationPeriodicCount.get());
    assertEquals(2, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

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

    assertEquals(1, robot.m_simulationStartCount.get());
    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_autonomousStartCount.get());
    assertEquals(0, robot.m_teleopStartCount.get());
    assertEquals(0, robot.m_utilityStartCount.get());

    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(0, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationStartCount.get());
    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(1, robot.m_autonomousStartCount.get());
    assertEquals(0, robot.m_teleopStartCount.get());
    assertEquals(0, robot.m_utilityStartCount.get());

    assertEquals(1, robot.m_robotPeriodicCount.get());
    assertEquals(1, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(1, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationStartCount.get());
    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(1, robot.m_autonomousStartCount.get());
    assertEquals(0, robot.m_teleopStartCount.get());
    assertEquals(0, robot.m_utilityStartCount.get());

    assertEquals(2, robot.m_robotPeriodicCount.get());
    assertEquals(2, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(2, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

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

    assertEquals(1, robot.m_simulationStartCount.get());
    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_autonomousStartCount.get());
    assertEquals(0, robot.m_teleopStartCount.get());
    assertEquals(0, robot.m_utilityStartCount.get());

    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(0, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationStartCount.get());
    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_autonomousStartCount.get());
    assertEquals(1, robot.m_teleopStartCount.get());
    assertEquals(0, robot.m_utilityStartCount.get());

    assertEquals(1, robot.m_robotPeriodicCount.get());
    assertEquals(1, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(1, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationStartCount.get());
    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_autonomousStartCount.get());
    assertEquals(1, robot.m_teleopStartCount.get());
    assertEquals(0, robot.m_utilityStartCount.get());

    assertEquals(2, robot.m_robotPeriodicCount.get());
    assertEquals(2, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(2, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

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

    assertEquals(1, robot.m_simulationStartCount.get());
    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_autonomousStartCount.get());
    assertEquals(0, robot.m_teleopStartCount.get());
    assertEquals(0, robot.m_utilityStartCount.get());

    assertEquals(0, robot.m_robotPeriodicCount.get());
    assertEquals(0, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(0, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationStartCount.get());
    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_autonomousStartCount.get());
    assertEquals(0, robot.m_teleopStartCount.get());
    assertEquals(1, robot.m_utilityStartCount.get());

    assertEquals(1, robot.m_robotPeriodicCount.get());
    assertEquals(1, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(1, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_simulationStartCount.get());
    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_autonomousStartCount.get());
    assertEquals(0, robot.m_teleopStartCount.get());
    assertEquals(1, robot.m_utilityStartCount.get());

    assertEquals(2, robot.m_robotPeriodicCount.get());
    assertEquals(2, robot.m_simulationPeriodicCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(2, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(0.02);

    assertEquals(1, robot.m_simulationStartCount.get());
    assertEquals(1, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_autonomousStartCount.get());
    assertEquals(0, robot.m_teleopStartCount.get());
    assertEquals(1, robot.m_utilityStartCount.get());

    assertEquals(3, robot.m_robotPeriodicCount.get());
    assertEquals(3, robot.m_simulationPeriodicCount.get());
    assertEquals(1, robot.m_disabledPeriodicCount.get());
    assertEquals(0, robot.m_autonomousPeriodicCount.get());
    assertEquals(0, robot.m_teleopPeriodicCount.get());
    assertEquals(2, robot.m_utilityPeriodicCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(1, robot.m_utilityEndCount.get());

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

    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_autonomousStartCount.get());
    assertEquals(0, robot.m_teleopStartCount.get());
    assertEquals(0, robot.m_utilityStartCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_autonomousStartCount.get());
    assertEquals(0, robot.m_teleopStartCount.get());
    assertEquals(0, robot.m_utilityStartCount.get());

    assertEquals(0, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

    // Transition to autonomous
    DriverStationSim.setEnabled(true);
    DriverStationSim.setRobotMode(RobotMode.AUTONOMOUS);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_disabledStartCount.get());
    assertEquals(1, robot.m_autonomousStartCount.get());
    assertEquals(0, robot.m_teleopStartCount.get());
    assertEquals(0, robot.m_utilityStartCount.get());

    assertEquals(1, robot.m_disabledEndCount.get());
    assertEquals(0, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

    // Transition to teleop
    DriverStationSim.setEnabled(true);
    DriverStationSim.setRobotMode(RobotMode.TELEOPERATED);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_disabledStartCount.get());
    assertEquals(1, robot.m_autonomousStartCount.get());
    assertEquals(1, robot.m_teleopStartCount.get());
    assertEquals(0, robot.m_utilityStartCount.get());

    assertEquals(1, robot.m_disabledEndCount.get());
    assertEquals(1, robot.m_autonomousEndCount.get());
    assertEquals(0, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

    // Transition to utility
    DriverStationSim.setEnabled(true);
    DriverStationSim.setRobotMode(RobotMode.UTILITY);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(PERIOD);

    assertEquals(1, robot.m_disabledStartCount.get());
    assertEquals(1, robot.m_autonomousStartCount.get());
    assertEquals(1, robot.m_teleopStartCount.get());
    assertEquals(1, robot.m_utilityStartCount.get());

    assertEquals(1, robot.m_disabledEndCount.get());
    assertEquals(1, robot.m_autonomousEndCount.get());
    assertEquals(1, robot.m_teleopEndCount.get());
    assertEquals(0, robot.m_utilityEndCount.get());

    // Transition to disabled
    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(PERIOD);

    assertEquals(2, robot.m_disabledStartCount.get());
    assertEquals(1, robot.m_autonomousStartCount.get());
    assertEquals(1, robot.m_teleopStartCount.get());
    assertEquals(1, robot.m_utilityStartCount.get());

    assertEquals(1, robot.m_disabledEndCount.get());
    assertEquals(1, robot.m_autonomousEndCount.get());
    assertEquals(1, robot.m_teleopEndCount.get());
    assertEquals(1, robot.m_utilityEndCount.get());

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

    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, callbackCount.get());

    SimHooks.stepTiming(PERIOD / 2.0);

    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(1, callbackCount.get());

    SimHooks.stepTiming(PERIOD / 2.0);

    assertEquals(1, robot.m_disabledStartCount.get());
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

    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, callbackCount.get());

    SimHooks.stepTiming(PERIOD * 3.0 / 8.0);

    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(0, callbackCount.get());

    SimHooks.stepTiming(PERIOD * 3.0 / 8.0);

    assertEquals(0, robot.m_disabledStartCount.get());
    assertEquals(0, robot.m_disabledPeriodicCount.get());
    assertEquals(1, callbackCount.get());

    SimHooks.stepTiming(PERIOD / 4.0);

    assertEquals(1, robot.m_disabledStartCount.get());
    assertEquals(1, robot.m_disabledPeriodicCount.get());
    assertEquals(1, callbackCount.get());

    SimHooks.stepTiming(PERIOD / 4.0);

    assertEquals(1, robot.m_disabledStartCount.get());
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
