// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.wpilibj.interfaces.Accelerometer;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import java.util.Arrays;
import java.util.Collection;
import java.util.logging.Logger;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;

public class BuiltInAccelerometerTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(BuiltInAccelerometerTest.class.getName());
  private static final double kAccelerationTolerance = 0.1;

  @BeforeAll
  public static void waitASecond() {
    /*
     * The testbench sometimes shakes a little from a previous test. Give it
     * some time.
     */
    Timer.delay(1.0);
  }

  /** Test with all valid ranges to make sure unpacking is always done correctly. */
  public static Collection<Accelerometer.Range[]> generateData() {
    return Arrays.asList(
        new Accelerometer.Range[][] {
          {Accelerometer.Range.k2G}, {Accelerometer.Range.k4G}, {Accelerometer.Range.k8G}
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
  @ParameterizedTest
  @MethodSource("generateData")
  public void testAccelerometer(Accelerometer.Range range) {
    var accelerometer = new BuiltInAccelerometer(range);
    assertEquals(0.0, accelerometer.getX(), kAccelerationTolerance);
    assertEquals(1.0, accelerometer.getY(), kAccelerationTolerance);
    assertEquals(0.0, accelerometer.getZ(), kAccelerationTolerance);
    accelerometer.close();
  }
}
