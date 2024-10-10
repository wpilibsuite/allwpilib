// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;

@ResourceLock("timing")
class MotorSafetyTest {
  AtomicInteger m_counter;
  MotorSafety m_motorSafety;

  @BeforeEach
  void setUp() {
    HAL.initialize(500, 0);
    m_counter = new AtomicInteger();
    m_motorSafety =
        new MotorSafety() {
          {
            this.setSafetyEnabled(true);
          }

          @Override
          public void stopMotor() {
            m_counter.incrementAndGet();
          }

          @Override
          public String getDescription() {
            return "test";
          }
        };
  }

  @AfterEach
  void tearDown() {
    m_motorSafety.close();
    m_motorSafety = null;
    m_counter = null;
  }

  @Test
  void monitorTest() {
    DriverStationSim.setEnabled(true);
    DriverStationSim.setAutonomous(false);
    DriverStationSim.setTest(false);
    DriverStationSim.notifyNewData();

    m_motorSafety.feed();
    assertTrue(m_motorSafety.isAlive());
    assertEquals(0, m_counter.get());

    for (int i = 0; i <= 6; i++) {
      SimHooks.stepTiming(0.02);
      DriverStationSim.notifyNewData();
    }

    forceContextSwitch();
    assertFalse(m_motorSafety.isAlive());
    assertEquals(1, m_counter.get());
  }

  /**
   * Sometimes during tests the assertions occur before a context switch to the MotorSafety thread,
   * so this forces the context switch.
   *
   * <p>Outside of unit tests, this shouldn't be a problem.
   */
  private static void forceContextSwitch() {
    try {
      Thread.sleep(10);
    } catch (InterruptedException e) {
      fail(e);
    }
  }
}
