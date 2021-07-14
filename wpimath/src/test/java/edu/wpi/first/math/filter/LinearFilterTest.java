// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.filter;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.params.provider.Arguments.arguments;

import java.util.Random;
import java.util.function.DoubleFunction;
import java.util.stream.Stream;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

class LinearFilterTest {
  private static final double kFilterStep = 0.005;
  private static final double kFilterTime = 2.0;
  private static final double kSinglePoleIIRTimeConstant = 0.015915;
  private static final double kHighPassTimeConstant = 0.006631;
  private static final int kMovAvgTaps = 6;

  private static final double kSinglePoleIIRExpectedOutput = -3.2172003;
  private static final double kHighPassExpectedOutput = 10.074717;
  private static final double kMovAvgExpectedOutput = -10.191644;

  private static double getData(double t) {
    return 100.0 * Math.sin(2.0 * Math.PI * t) + 20.0 * Math.cos(50.0 * Math.PI * t);
  }

  private static double getPulseData(double t) {
    if (Math.abs(t - 1.0) < 0.001) {
      return 1.0;
    } else {
      return 0.0;
    }
  }

  @Test
  void illegalTapNumberTest() {
    assertThrows(IllegalArgumentException.class, () -> LinearFilter.movingAverage(0));
  }

  /** Test if the filter reduces the noise produced by a signal generator. */
  @ParameterizedTest
  @MethodSource("noiseFilterProvider")
  void noiseReduceTest(final LinearFilter filter) {
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

    assertTrue(
        noiseGenError > filterError,
        "Filter should have reduced noise accumulation from "
            + noiseGenError
            + " but failed. The filter error was "
            + filterError);
  }

  static Stream<LinearFilter> noiseFilterProvider() {
    return Stream.of(
        LinearFilter.singlePoleIIR(kSinglePoleIIRTimeConstant, kFilterStep),
        LinearFilter.movingAverage(kMovAvgTaps));
  }

  /** Test if the linear filters produce consistent output for a given data set. */
  @ParameterizedTest
  @MethodSource("outputFilterProvider")
  void outputTest(
      final LinearFilter filter, final DoubleFunction<Double> data, final double expectedOutput) {
    double filterOutput = 0.0;
    for (double t = 0.0; t < kFilterTime; t += kFilterStep) {
      filterOutput = filter.calculate(data.apply(t));
    }

    assertEquals(expectedOutput, filterOutput, 5e-5, "Filter output was incorrect.");
  }

  static Stream<Arguments> outputFilterProvider() {
    return Stream.of(
        arguments(
            LinearFilter.singlePoleIIR(kSinglePoleIIRTimeConstant, kFilterStep),
            (DoubleFunction<Double>) LinearFilterTest::getData,
            kSinglePoleIIRExpectedOutput),
        arguments(
            LinearFilter.highPass(kHighPassTimeConstant, kFilterStep),
            (DoubleFunction<Double>) LinearFilterTest::getData,
            kHighPassExpectedOutput),
        arguments(
            LinearFilter.movingAverage(kMovAvgTaps),
            (DoubleFunction<Double>) LinearFilterTest::getData,
            kMovAvgExpectedOutput),
        arguments(
            LinearFilter.movingAverage(kMovAvgTaps),
            (DoubleFunction<Double>) LinearFilterTest::getPulseData,
            0.0));
  }

  /** Test backward finite difference. */
  @Test
  void backwardFiniteDifferenceTest() {
    double h = 0.05;

    assertResults(
        // f(x) = x^2
        (double x) -> x * x,
        // df/dx = 2x
        (double x) -> 2.0 * x,
        h,
        -20.0,
        20.0);

    assertResults(
        // f(x) = sin(x)
        (double x) -> Math.sin(x),
        // df/dx = cos(x)
        (double x) -> Math.cos(x),
        h,
        -20.0,
        20.0);

    assertResults(
        // f(x) = ln(x)
        (double x) -> Math.log(x),
        // df/dx = 1 / x
        (double x) -> 1.0 / x,
        h,
        1.0,
        20.0);
  }

  /**
   * Helper for checking results of backward finite difference.
   *
   * @param f Function of which to take derivative.
   * @param dfdx Derivative of f.
   * @param h Sample period in seconds.
   * @param min Minimum of f's domain to test.
   * @param max Maximum of f's domain to test.
   */
  void assertResults(
      DoubleFunction<Double> f, DoubleFunction<Double> dfdx, double h, double min, double max) {
    var filter = LinearFilter.backwardFiniteDifference(1, 2, h);

    for (int i = (int) (min / h); i < (int) (max / h); ++i) {
      // Let filter initialize
      if (i < (int) (min / h) + 2) {
        filter.calculate(f.apply(i * h));
        continue;
      }

      // The order of accuracy is O(h^(N - d)) where N is number of stencil
      // points and d is order of derivative
      assertEquals(
          dfdx.apply(i * h),
          filter.calculate(f.apply(i * h)),
          Math.max(Math.pow(h, 2 - 1), 1e-7) + 1e-5);
    }
  }
}
