/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.Random;
import java.util.stream.Stream;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;

import static org.junit.jupiter.api.Assertions.assertTrue;

class LinearFilterNoiseTest {
  private static final double kFilterStep = 0.005;
  private static final double kFilterTime = 2.0;
  private static final double kSinglePoleIIRTimeConstant = 0.015915;
  private static final int kMovAvgTaps = 6;

  @SuppressWarnings("ParameterName")
  private static double getData(double t) {
    return 100.0 * Math.sin(2.0 * Math.PI * t);
  }

  /**
   * Test if the filter reduces the noise produced by a signal generator.
   */
  @ParameterizedTest
  @MethodSource("filterProvider")
  void testNoiseReduce(final LinearFilter filter) {
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

  static Stream<LinearFilter> filterProvider() {
    return Stream.of(
        LinearFilter.singlePoleIIR(kSinglePoleIIRTimeConstant, kFilterStep),
        LinearFilter.movingAverage(kMovAvgTaps)
    );
  }
}
