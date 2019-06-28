/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.Random;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;

import static org.junit.jupiter.api.Assertions.assertTrue;


public class LinearFilterNoiseTest {
  public enum TestType {
    kSinglePoleIIR, kMovAvg
  }

  // Filter constants
  public static final double kFilterStep = 0.005;
  public static final double kFilterTime = 2.0;
  public static final double kSinglePoleIIRTimeConstant = 0.015915;
  public static final int kMovAvgTaps = 6;

  @SuppressWarnings("ParameterName")
  public static double getData(double t) {
    return 100.0 * Math.sin(2.0 * Math.PI * t);
  }

  /**
   * Test if the filter reduces the noise produced by a signal generator.
   */
  @ParameterizedTest
  @EnumSource(TestType.class)
  public void testNoiseReduce(TestType type) {
    final LinearFilter filter;

    if (type == TestType.kSinglePoleIIR) {
      filter = LinearFilter.singlePoleIIR(kSinglePoleIIRTimeConstant, kFilterStep);
    } else {
      filter = LinearFilter.movingAverage(kMovAvgTaps);
    }

    double noiseGenError = 0.0;
    double filterError = 0.0;

    final Random gen = new Random();
    final double kStdDev = 10.0;

    for (double t = 0; t < kFilterTime; t += kFilterStep) {
      final double theory = getData(t);
      final double noise = gen.nextGaussian() * kStdDev;
      filterError += Math.abs(filter.calculate(theory + noise) - theory);
      noiseGenError += Math.abs(noise - theory);
    }

    assertTrue(noiseGenError > filterError,
        "Filter should have reduced noise accumulation from " + noiseGenError
        + " but failed. The filter error was " + filterError);
  }
}
