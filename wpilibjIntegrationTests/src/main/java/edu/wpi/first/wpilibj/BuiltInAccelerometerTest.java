/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import java.util.Arrays;
import java.util.Collection;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.interfaces.Accelerometer;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;

import static org.junit.Assert.assertEquals;

@RunWith(Parameterized.class)
public class BuiltInAccelerometerTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(BuiltInAccelerometerTest.class.getName());
  private static final double kAccelerationTolerance = 0.1;
  private final BuiltInAccelerometer m_accelerometer;

  public BuiltInAccelerometerTest(Accelerometer.Range range) {
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

  /**
   * Test with all valid ranges to make sure unpacking is always done correctly.
   */
  @Parameters
  public static Collection<Accelerometer.Range[]> generateData() {
    return Arrays.asList(new Accelerometer.Range[][]{{Accelerometer.Range.k2G},
        {Accelerometer.Range.k4G}, {Accelerometer.Range.k8G}});
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
