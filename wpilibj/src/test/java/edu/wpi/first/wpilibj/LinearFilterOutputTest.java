/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.function.DoubleFunction;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;

import static org.junit.jupiter.api.Assertions.assertEquals;


public class LinearFilterOutputTest {
  public enum TestType {
    kSinglePoleIIR,
    kHighPass,
    kMovAvg,
    kPulse
  }

  // Filter constants
  public static final double kFilterStep = 0.005;
  public static final double kFilterTime = 2.0;
  public static final double kSinglePoleIIRTimeConstant = 0.015915;
  public static final double kSinglePoleIIRExpectedOutput = -3.2172003;
  public static final double kHighPassTimeConstant = 0.006631;
  public static final double kHighPassExpectedOutput = 10.074717;
  public static final int kMovAvgTaps = 6;
  public static final double kMovAvgExpectedOutput = -10.191644;

  @SuppressWarnings("ParameterName")
  public static double getData(double t) {
    return 100.0 * Math.sin(2.0 * Math.PI * t) + 20.0 * Math.cos(50.0 * Math.PI * t);
  }

  @SuppressWarnings("ParameterName")
  public static double getPulseData(double t) {
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
  @EnumSource(TestType.class)
  public void testOutput(TestType type) {
    final LinearFilter filter;
    final DoubleFunction<Double> data;
    final double expectedOutput;

    if (type == TestType.kSinglePoleIIR) {
      filter = LinearFilter.singlePoleIIR(kSinglePoleIIRTimeConstant, kFilterStep);
      data = (double t) -> getData(t);
      expectedOutput = kSinglePoleIIRExpectedOutput;
    } else if (type == TestType.kHighPass) {
      filter = LinearFilter.highPass(kHighPassTimeConstant, kFilterStep);
      data = (double t) -> getData(t);
      expectedOutput = kHighPassExpectedOutput;
    } else if (type == TestType.kMovAvg) {
      filter = LinearFilter.movingAverage(kMovAvgTaps);
      data = (double t) -> getData(t);
      expectedOutput = kMovAvgExpectedOutput;
    } else {
      filter = LinearFilter.movingAverage(kMovAvgTaps);
      data = (double t) -> getPulseData(t);
      expectedOutput = 0.0;
    }

    double filterOutput = 0.0;
    for (double t = 0.0; t < kFilterTime; t += kFilterStep) {
      filterOutput = filter.calculate(data.apply(t));
    }

    assertEquals(expectedOutput, filterOutput, 0.00005, "Filter output was incorrect.");
  }
}
