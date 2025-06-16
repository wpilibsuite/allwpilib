// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import java.util.Collection;
import java.util.List;
import java.util.logging.Logger;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

@RunWith(Parameterized.class)
public class BuiltInAccelerometerTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(BuiltInAccelerometerTest.class.getName());
  private static final double kAccelerationTolerance = 0.1;
  private final BuiltInAccelerometer m_accelerometer;

  public BuiltInAccelerometerTest(BuiltInAccelerometer.Range range) {
    m_accelerometer = new BuiltInAccelerometer(range);
  }

  @BeforeClass
  public static void waitASecond() {
    /*
     * The testbench sometimes shakes a little from a previous test. Give it
     * some time.
     */
    Timer.delay(1.0);
  }

  /** Test with all valid ranges to make sure unpacking is always done correctly. */
  @Parameters
  public static Collection<BuiltInAccelerometer.Range[]> generateData() {
    return List.of(new BuiltInAccelerometer.Range[][] {
      {BuiltInAccelerometer.Range.k2G},
      {BuiltInAccelerometer.Range.k4G},
      {BuiltInAccelerometer.Range.k8G}
    });
  }

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  /**
   * There's not much we can automatically test with the on-board accelerometer, but checking for
   * gravity is probably good enough to tell that it's working.
   */
  @Test
  public void testAccelerometer() {
    assertEquals(0.0, m_accelerometer.getX(), kAccelerationTolerance);
    assertEquals(1.0, m_accelerometer.getY(), kAccelerationTolerance);
    assertEquals(0.0, m_accelerometer.getZ(), kAccelerationTolerance);
  }
}
