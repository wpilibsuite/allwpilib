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

  /** Test central finite difference. */
  @Test
  void centralFiniteDifferenceTest() {
    double h = 0.005;

    assertCentralResults(
        1,
        3,
        // f(x) = x²
        (double x) -> x * x,
        // df/dx = 2x
        (double x) -> 2.0 * x,
        h,
        -20.0,
        20.0);

    assertCentralResults(
        1,
        3,
        // f(x) = sin(x)
        (double x) -> Math.sin(x),
        // df/dx = cos(x)
        (double x) -> Math.cos(x),
        h,
        -20.0,
        20.0);

    assertCentralResults(
        1,
        3,
        // f(x) = ln(x)
        (double x) -> Math.log(x),
        // df/dx = 1/x
        (double x) -> 1.0 / x,
        h,
        1.0,
        20.0);

    assertCentralResults(
        2,
        5,
        // f(x) = x²
        (double x) -> x * x,
        // d²f/dx² = 2
        (double x) -> 2.0,
        h,
        -20.0,
        20.0);

    assertCentralResults(
        2,
        5,
        // f(x) = sin(x)
        (double x) -> Math.sin(x),
        // d²f/dx² = -sin(x)
        (double x) -> -Math.sin(x),
        h,
        -20.0,
        20.0);

    assertCentralResults(
        2,
        5,
        // f(x) = ln(x)
        (double x) -> Math.log(x),
        // d²f/dx² = -1/x²
        (double x) -> -1.0 / (x * x),
        h,
        1.0,
        20.0);
  }

  /** Test backward finite difference. */
  @Test
  void backwardFiniteDifferenceTest() {
    double h = 0.005;

    assertBackwardResults(
        1,
        2,
        // f(x) = x²
        (double x) -> x * x,
        // df/dx = 2x
        (double x) -> 2.0 * x,
        h,
        -20.0,
        20.0);

    assertBackwardResults(
        1,
        2,
        // f(x) = sin(x)
        (double x) -> Math.sin(x),
        // df/dx = cos(x)
        (double x) -> Math.cos(x),
        h,
        -20.0,
        20.0);

    assertBackwardResults(
        1,
        2,
        // f(x) = ln(x)
        (double x) -> Math.log(x),
        // df/dx = 1/x
        (double x) -> 1.0 / x,
        h,
        1.0,
        20.0);

    assertBackwardResults(
        2,
        4,
        // f(x) = x²
        (double x) -> x * x,
        // d²f/dx² = 2
        (double x) -> 2.0,
        h,
        -20.0,
        20.0);

    assertBackwardResults(
        2,
        4,
        // f(x) = sin(x)
        (double x) -> Math.sin(x),
        // d²f/dx² = -sin(x)
        (double x) -> -Math.sin(x),
        h,
        -20.0,
        20.0);

    assertBackwardResults(
        2,
        4,
        // f(x) = ln(x)
        (double x) -> Math.log(x),
        // d²f/dx² = -1/x²
        (double x) -> -1.0 / (x * x),
        h,
        1.0,
        20.0);
  }

  /**
   * Helper for checking results of central finite difference.
   *
   * @param derivative The order of the derivative.
   * @param samples The number of sample points.
   * @param f Function of which to take derivative.
   * @param dfdx Derivative of f.
   * @param h Sample period in seconds.
   * @param min Minimum of f's domain to test.
   * @param max Maximum of f's domain to test.
   */
  void assertCentralResults(
      int derivative,
      int samples,
      DoubleFunction<Double> f,
      DoubleFunction<Double> dfdx,
      double h,
      double min,
      double max) {
    if (samples % 2 == 0) {
      throw new IllegalArgumentException("Number of samples must be odd.");
    }

    // Generate stencil points from -(samples - 1)/2 to (samples - 1)/2
    int[] stencil = new int[samples];
    for (int i = 0; i < samples; ++i) {
      stencil[i] = -(samples - 1) / 2 + i;
    }

    var filter = LinearFilter.finiteDifference(derivative, stencil, h);

    for (int i = (int) (min / h); i < (int) (max / h); ++i) {
      // Let filter initialize
      if (i < (int) (min / h) + samples) {
        filter.calculate(f.apply(i * h));
        continue;
      }

      // The order of accuracy is O(h^(N - d)) where N is number of stencil
      // points and d is order of derivative
      assertEquals(
          dfdx.apply((i - samples / 2) * h),
          filter.calculate(f.apply(i * h)),
          Math.pow(h, samples - derivative));
    }
  }

  /**
   * Helper for checking results of backward finite difference.
   *
   * @param derivative The order of the derivative.
   * @param samples The number of sample points.
   * @param f Function of which to take derivative.
   * @param dfdx Derivative of f.
   * @param h Sample period in seconds.
   * @param min Minimum of f's domain to test.
   * @param max Maximum of f's domain to test.
   */
  void assertBackwardResults(
      int derivative,
      int samples,
      DoubleFunction<Double> f,
      DoubleFunction<Double> dfdx,
      double h,
      double min,
      double max) {
    var filter = LinearFilter.backwardFiniteDifference(derivative, samples, h);

    for (int i = (int) (min / h); i < (int) (max / h); ++i) {
      // Let filter initialize
      if (i < (int) (min / h) + samples) {
        filter.calculate(f.apply(i * h));
        continue;
      }

      // For central finite difference, the derivative computed at this point is
      // half the window size in the past.
      // The order of accuracy is O(h^(N - d)) where N is number of stencil
      // points and d is order of derivative
      assertEquals(
          dfdx.apply(i * h),
          filter.calculate(f.apply(i * h)),
          10.0 * Math.pow(h, samples - derivative));
    }
  }
}
