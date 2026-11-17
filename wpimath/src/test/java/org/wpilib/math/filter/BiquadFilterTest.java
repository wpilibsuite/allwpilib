// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.filter;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Random;
import org.junit.jupiter.api.Test;

class BiquadFilterTest {
  @Test
  void passThroughTest() {
    BiquadFilter filter = new BiquadFilter(new BiquadFilter.Section(1.0, 0.0, 0.0, 0.0, 0.0));
    Random rng = new Random(42);
    for (int i = 0; i < 200; i++) {
      double x = (rng.nextDouble() - 0.5) * 200.0;
      assertEquals(x, filter.calculate(x), 0.0);
    }
  }

  @Test
  void firstOrderMatchesSinglePoleIIRTest() {
    // SinglePoleIIR: y[n] = (1-g) x[n] + g y[n-1], g = exp(-dt/T)
    // As biquad: {1-g, 0, 0, -g, 0}
    final double timeConstant = 0.015915;
    final double period = 0.005;
    final double g = Math.exp(-period / timeConstant);

    LinearFilter linear = LinearFilter.singlePoleIIR(timeConstant, period);
    BiquadFilter biquad = new BiquadFilter(new BiquadFilter.Section(1.0 - g, 0.0, 0.0, -g, 0.0));

    Random rng = new Random(7);
    for (int i = 0; i < 1000; i++) {
      double x = (rng.nextDouble() - 0.5) * 100.0;
      double yLin = linear.calculate(x);
      double yBiq = biquad.calculate(x);
      assertEquals(yLin, yBiq, 1e-12, "sample " + i);
    }
  }

  @Test
  void butterworth4thOrderLowPassTest() {
    // scipy.signal.butter(4, 50.0, btype='low', fs=1000.0, output='sos')
    BiquadFilter filter =
        new BiquadFilter(
            new BiquadFilter.Section(
                0.00041659920440659937,
                0.0008331984088131987,
                0.00041659920440659937,
                -1.4796742169311934,
                0.5558215432824889),
            new BiquadFilter.Section(1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979));

    double[] expected = {
      0.00041659920440659937,
      0.0029914483065925663,
      0.010405740533503665,
      0.024092655231875183,
      0.04300386328531425,
      0.06442081415630327,
      0.08518000836484753,
      0.10245740377665029,
      0.1142030744642985,
      0.11931076610150239,
      0.11759868177262096,
      0.10966797135549569,
      0.09669445862739445,
      0.08019770689053446,
      0.06182021082200691,
      0.04313888252371942,
      0.025521549440937964,
      0.01003324447867498,
      -0.002609160074363505,
      -0.01203989315971688,
      -0.018213508615082776,
      -0.021350392922805498,
      -0.02186860712506684,
      -0.020311212598085788,
      -0.01727668431352673,
      -0.013358111475758645,
      -0.009094876704322833,
      -0.004938595712161598,
      -0.0012334395430879353,
      0.0017903545884787877,
    };

    for (int i = 0; i < expected.length; i++) {
      double x = (i == 0) ? 1.0 : 0.0;
      double y = filter.calculate(x);
      assertEquals(expected[i], y, 1e-10, "sample " + i);
    }
  }

  @Test
  void notch60HzTest() {
    // scipy.signal.iirnotch(60, Q=10, fs=1000) via tf2sos
    BiquadFilter filter =
        new BiquadFilter(
            new BiquadFilter.Section(
                0.9814970254751076,
                -1.8251457105120343,
                0.9814970254751076,
                -1.8251457105120341,
                0.9629940509502151));

    final double fs = 1000.0;
    final int samples = 1000;
    double[] output = new double[samples];
    double[] input = new double[samples];
    for (int n = 0; n < samples; n++) {
      double t = n / fs;
      input[n] = Math.sin(2.0 * Math.PI * 10.0 * t) + Math.sin(2.0 * Math.PI * 60.0 * t);
      output[n] = filter.calculate(input[n]);
    }

    assertEquals(-0.017355123579818322, output[500], 1e-10);
    assertEquals(-0.08007594066581347, output[999], 1e-10);

    // Basic DFT at 10 Hz and 60 Hz over steady-state window.
    final int window = 512;
    double atten60 = attenuationDb(output, input, 60.0, fs, samples - window, window);
    double atten10 = attenuationDb(output, input, 10.0, fs, samples - window, window);

    assertTrue(atten60 < -40.0, "60 Hz attenuation too weak: " + atten60);
    assertTrue(atten10 > -0.5, "10 Hz passband loss too large: " + atten10);
  }

  @Test
  void order8ButterworthMatchesScipyTest() {
    // scipy.signal.butter(8, 100.0, btype='low', fs=1000.0, output='sos')
    BiquadFilter filter =
        new BiquadFilter(
            new BiquadFilter.Section(
                2.3959644103776166e-05,
                4.791928820755233e-05,
                2.3959644103776166e-05,
                -1.0263514742610553,
                0.26864019099379005),
            new BiquadFilter.Section(1.0, 2.0, 1.0, -1.0868584613628944, 0.343430940165366),
            new BiquadFilter.Section(1.0, 2.0, 1.0, -1.2197253651240232, 0.5076634651740437),
            new BiquadFilter.Section(1.0, 2.0, 1.0, -1.4515795942478362, 0.794251053241888));

    final int N = 500;
    final double fs = 1000.0;
    final double f0 = 1.0;
    final double f1 = 200.0;
    final double t1 = (N - 1) / fs;
    final double k = (f1 - f0) / t1;

    int[] spotIdx = {10, 50, 100, 250, 499};
    double[] expected = {
      0.8950675041062186,
      -0.7902247252134351,
      0.1716891991372734,
      0.05240058121316523,
      -0.0016952227415119995,
    };
    double[] got = new double[spotIdx.length];

    int j = 0;
    for (int n = 0; n < N; n++) {
      double t = n / fs;
      double phase = 2.0 * Math.PI * (f0 * t + 0.5 * k * t * t);
      double x = Math.cos(phase);
      double y = filter.calculate(x);
      if (j < spotIdx.length && n == spotIdx[j]) {
        got[j++] = y;
      }
    }

    for (int i = 0; i < expected.length; i++) {
      assertEquals(expected[i], got[i], 1e-10, "sample index " + spotIdx[i]);
    }
  }

  @Test
  void resetZerosStateTest() {
    BiquadFilter filter =
        new BiquadFilter(
            new BiquadFilter.Section(
                0.00041659920440659937,
                0.0008331984088131987,
                0.00041659920440659937,
                -1.4796742169311934,
                0.5558215432824889),
            new BiquadFilter.Section(1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979));

    for (int i = 0; i < 50; i++) {
      filter.calculate(1.0);
    }
    assertNotEquals(0.0, filter.lastValue());

    filter.reset();
    assertEquals(0.0, filter.lastValue(), 0.0);

    double y = filter.calculate(1.0);
    assertEquals(0.00041659920440659937, y, 1e-12);
  }

  @Test
  void resetToSteadyStateTest() {
    BiquadFilter filter =
        new BiquadFilter(
            new BiquadFilter.Section(
                0.00041659920440659937,
                0.0008331984088131987,
                0.00041659920440659937,
                -1.4796742169311934,
                0.5558215432824889),
            new BiquadFilter.Section(1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979));

    final double input = 3.0;
    filter.reset(input);

    assertEquals(input, filter.lastValue(), 1e-12);
    assertEquals(input, filter.calculate(input), 1e-12);
    for (int i = 0; i < 20; i++) {
      assertEquals(input, filter.calculate(input), 1e-12);
    }
  }

  @Test
  void dcGainConvergesTest() {
    BiquadFilter filter =
        new BiquadFilter(
            new BiquadFilter.Section(
                0.00041659920440659937,
                0.0008331984088131987,
                0.00041659920440659937,
                -1.4796742169311934,
                0.5558215432824889),
            new BiquadFilter.Section(1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979));

    final double input = 2.5;
    double y = 0.0;
    for (int i = 0; i < 500; i++) {
      y = filter.calculate(input);
    }
    assertEquals(input, y, 1e-6);
  }

  @Test
  void numSectionsTest() {
    BiquadFilter one = new BiquadFilter(new BiquadFilter.Section(1.0, 0.0, 0.0, 0.0, 0.0));
    assertEquals(1, one.numSections());

    BiquadFilter three =
        new BiquadFilter(
            new BiquadFilter.Section(1.0, 0.0, 0.0, 0.0, 0.0),
            new BiquadFilter.Section(1.0, 0.0, 0.0, 0.0, 0.0),
            new BiquadFilter.Section(1.0, 0.0, 0.0, 0.0, 0.0));
    assertEquals(3, three.numSections());
  }

  @Test
  void emptyCascadeThrowsTest() {
    assertThrows(IllegalArgumentException.class, () -> new BiquadFilter());
  }

  private static double attenuationDb(
      double[] out, double[] in, double freq, double fs, int start, int window) {
    double magOut = dftMag(out, freq, fs, start, window);
    double magIn = dftMag(in, freq, fs, start, window);
    return 20.0 * Math.log10(magOut / magIn);
  }

  private static double dftMag(double[] sig, double freq, double fs, int start, int window) {
    double re = 0.0;
    double im = 0.0;
    for (int n = 0; n < window; n++) {
      double phase = 2.0 * Math.PI * freq * n / fs;
      re += sig[start + n] * Math.cos(phase);
      im -= sig[start + n] * Math.sin(phase);
    }
    return Math.hypot(re, im);
  }
}
