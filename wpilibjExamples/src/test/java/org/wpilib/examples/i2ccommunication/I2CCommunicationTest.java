// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.i2ccommunication;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTimeoutPreemptively;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.time.Duration;
import java.util.concurrent.CompletableFuture;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;
import org.junit.jupiter.params.provider.ValueSource;
import org.wpilib.driverstation.DriverStation;
import org.wpilib.hardware.hal.AllianceStationID;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.simulation.CallbackStore;
import org.wpilib.simulation.DriverStationSim;
import org.wpilib.simulation.I2CSim;
import org.wpilib.simulation.SimHooks;

@ResourceLock("timing")
class I2CCommunicationTest {
  private Robot m_robot;
  private Thread m_thread;
  private final I2CSim m_i2c = new I2CSim(Robot.kPort.value);
  private CompletableFuture<String> m_future;
  private CallbackStore m_callback;

  @BeforeEach
  void startThread() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
    DriverStationSim.resetData();
    m_future = new CompletableFuture<>();
    m_callback =
        m_i2c.registerWriteCallback(
            (name, buffer, count) -> m_future.complete(new String(buffer, 0, count)));
    m_robot = new Robot();
    m_thread = new Thread(m_robot::startCompetition);
    m_thread.start();
    SimHooks.stepTiming(0.0); // Wait for Notifiers
  }

  @AfterEach
  void stopThread() {
    m_robot.endCompetition();
    try {
      m_thread.interrupt();
      m_thread.join();
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    m_robot.close();
    m_callback.close();
    m_i2c.resetData();
  }

  @EnumSource(AllianceStationID.class)
  @ParameterizedTest(name = "alliance[{index}]: {0}")
  void allianceTest(AllianceStationID alliance) {
    DriverStationSim.setAllianceStationId(alliance);
    DriverStationSim.notifyNewData();

    assertTrue(m_i2c.getInitialized());

    SimHooks.stepTiming(0.02);

    String str = assertTimeoutPreemptively(Duration.ofMillis(20L), () -> m_future.get());
    char expected = alliance.name().startsWith("Red") ? 'R' : 'B';
    if (alliance.name().startsWith("Unknown")) {
      expected = 'U';
    }

    assertEquals(expected, str.charAt(0));
  }

  @ValueSource(booleans = {true, false})
  @ParameterizedTest(name = "enabled[{index}]: {0}")
  void enabledTest(boolean enabled) {
    DriverStationSim.setEnabled(enabled);
    DriverStationSim.notifyNewData();

    assertTrue(m_i2c.getInitialized());

    SimHooks.stepTiming(0.02);

    String str = assertTimeoutPreemptively(Duration.ofMillis(20L), () -> m_future.get());
    char expected = enabled ? 'E' : 'D';

    assertEquals(expected, str.charAt(1));
  }

  @ValueSource(booleans = {true, false})
  @ParameterizedTest(name = "autonomous[{index}]: {0}")
  void autonomousTest(boolean autonomous) {
    DriverStationSim.setAutonomous(autonomous);
    DriverStationSim.notifyNewData();

    assertTrue(m_i2c.getInitialized());

    SimHooks.stepTiming(0.02);

    String str = assertTimeoutPreemptively(Duration.ofMillis(20L), () -> m_future.get());
    char expected = autonomous ? 'A' : 'T';

    assertEquals(expected, str.charAt(2));
  }

  @ValueSource(doubles = {112.0, 45.0, 27.0, 23.0, 3.0})
  @ParameterizedTest(name = "matchTime[{index}]: {0}s")
  void matchTimeTest(double matchTime) {
    DriverStationSim.setMatchTime(matchTime);
    DriverStationSim.notifyNewData();
    assertTrue(m_i2c.getInitialized());

    SimHooks.stepTiming(0.02);

    String str = assertTimeoutPreemptively(Duration.ofMillis(20L), () -> m_future.get());
    String expected = String.format("%03d", (int) DriverStation.getMatchTime());

    assertEquals(expected, str.substring(3));
  }
}
