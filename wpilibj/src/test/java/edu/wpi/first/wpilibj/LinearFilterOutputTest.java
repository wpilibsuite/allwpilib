/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.function.DoubleFunction;
import java.util.stream.Stream;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.params.provider.Arguments.arguments;

class LinearFilterOutputTest {
  // Filter constants
  private static final double kFilterStep = 0.005;
  private static final double kFilterTime = 2.0;
  private static final double kSinglePoleIIRTimeConstant = 0.015915;
  private static final double kSinglePoleIIRExpectedOutput = -3.2172003;
  private static final double kHighPassTimeConstant = 0.006631;
  private static final double kHighPassExpectedOutput = 10.074717;
  private static final int kMovAvgTaps = 6;
  private static final double kMovAvgExpectedOutput = -10.191644;

  @SuppressWarnings("ParameterName")
  private static double getData(double t) {
    return 100.0 * Math.sin(2.0 * Math.PI * t) + 20.0 * Math.cos(50.0 * Math.PI * t);
  }

  @SuppressWarnings("ParameterName")
  private static double getPulseData(double t) {
    if (Math.abs(t - 1.0) < 0.001) {
      return 1.0;
    } else {
      return 0.0;
    }
  }

  /**
   * Test if the linear filters produce consistent output for a given data set.
   */
  @ParameterizedTest
  @MethodSource("filterProvider")
  void testOutput(final LinearFilter filter, final DoubleFunction<Double> data,
                         final double expectedOutput) {
    double filterOutput = 0.0;
    for (double t = 0.0; t < kFilterTime; t += kFilterStep) {
      filterOutput = filter.calculate(data.apply(t));
    }

    assertEquals(expectedOutput, filterOutput, 5e-5, "Filter output was incorrect.");
  }

  static Stream<Arguments> filterProvider() {
    return Stream.of(
        arguments(LinearFilter.singlePoleIIR(kSinglePoleIIRTimeConstant, kFilterStep),
            (DoubleFunction) LinearFilterOutputTest::getData,
            kSinglePoleIIRExpectedOutput),
        arguments(LinearFilter.highPass(kHighPassTimeConstant, kFilterStep),
            (DoubleFunction) LinearFilterOutputTest::getData,
            kHighPassExpectedOutput),
        arguments(LinearFilter.movingAverage(kMovAvgTaps),
            (DoubleFunction) LinearFilterOutputTest::getData,
            kMovAvgExpectedOutput),
        arguments(LinearFilter.movingAverage(kMovAvgTaps),
            (DoubleFunction) LinearFilterOutputTest::getPulseData,
            0.0)
    );
  }
}
