/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;

import java.util.Arrays;
import java.util.Collection;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertTrue;

@RunWith(Parameterized.class)
public class SpeedControllerGroupTest {
  private final SpeedController[] m_speedControllers;
  private final SpeedControllerGroup m_group;

  /**
   * Returns a Collection of ArrayLists with various MockSpeedController configurations.
   */
  @Parameterized.Parameters
  public static Collection<Object[][]> data() {
    return Arrays.asList((Object[][][]) new SpeedController[][][] {
        {{new MockSpeedController()}},
        {{new MockSpeedController(),
            new MockSpeedController()}},
        {{new MockSpeedController(),
            new MockSpeedController(),
            new MockSpeedController()}}
    });
  }

  /**
   * Construct SpeedControllerGroupTest.
   */
  public SpeedControllerGroupTest(SpeedController[] speedControllers) {
    m_group = new SpeedControllerGroup(speedControllers[0],
        Arrays.copyOfRange(speedControllers, 1, speedControllers.length));
    m_speedControllers = speedControllers;
  }

  @Test
  public void testSet() {
    m_group.set(1.0);

    assertArrayEquals(Arrays.stream(m_speedControllers).mapToDouble(__ -> 1.0).toArray(),
        Arrays.stream(m_speedControllers).mapToDouble(SpeedController::get).toArray(),
        0.0);
  }

  @Test
  public void testGetInverted() {
    m_group.setInverted(true);

    assertTrue(m_group.getInverted());
  }

  @Test
  public void testSetInvertedDoesNotModifySpeedControllers() {
    for (SpeedController speedController : m_speedControllers) {
      speedController.setInverted(false);
    }
    m_group.setInverted(true);

    assertArrayEquals(Arrays.stream(m_speedControllers).map(__ -> false).toArray(),
        Arrays.stream(m_speedControllers).map(SpeedController::getInverted).toArray());
  }

  @Test
  public void testSetInvertedDoesInvert() {
    m_group.setInverted(true);
    m_group.set(1.0);

    assertArrayEquals(Arrays.stream(m_speedControllers).mapToDouble(__ -> -1.0).toArray(),
        Arrays.stream(m_speedControllers).mapToDouble(SpeedController::get).toArray(),
        0.0);
  }

  @Test
  public void testDisable() {
    m_group.set(1.0);
    m_group.disable();

    assertArrayEquals(Arrays.stream(m_speedControllers).mapToDouble(__ -> 0.0).toArray(),
        Arrays.stream(m_speedControllers).mapToDouble(SpeedController::get).toArray(),
        0.0);
  }

  @Test
  public void testStopMotor() {
    m_group.set(1.0);
    m_group.stopMotor();

    assertArrayEquals(Arrays.stream(m_speedControllers).mapToDouble(__ -> 0.0).toArray(),
        Arrays.stream(m_speedControllers).mapToDouble(SpeedController::get).toArray(),
        0.0);
  }

  @Test
  public void testPidWrite() {
    m_group.pidWrite(1.0);

    assertArrayEquals(Arrays.stream(m_speedControllers).mapToDouble(__ -> 1.0).toArray(),
        Arrays.stream(m_speedControllers).mapToDouble(SpeedController::get).toArray(),
        0.0);
  }
}
