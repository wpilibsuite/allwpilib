// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.filter;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class BiquadFilterDesignTest {
  // |H(e^{j·2π·f/fs})| across a cascade of biquad sections, computed as a real
  // double via the standard digital-filter cascade-magnitude formula.
  private static double cascadeMagnitude(BiquadFilter.Section[] sos, double f, double fs) {
    double w = 2.0 * Math.PI * f / fs;
    double cos1 = Math.cos(w);
    double sin1 = -Math.sin(w);
    double cos2 = Math.cos(2.0 * w);
    double sin2 = -Math.sin(2.0 * w);
    double hRe = 1.0;
    double hIm = 0.0;
    for (BiquadFilter.Section s : sos) {
      double numRe = s.b0 + s.b1 * cos1 + s.b2 * cos2;
      double numIm = s.b1 * sin1 + s.b2 * sin2;
      double denRe = 1.0 + s.a1 * cos1 + s.a2 * cos2;
      double denIm = s.a1 * sin1 + s.a2 * sin2;
      double denMag = denRe * denRe + denIm * denIm;
      double tmpRe = (numRe * denRe + numIm * denIm) / denMag;
      double tmpIm = (numIm * denRe - numRe * denIm) / denMag;
      double newRe = hRe * tmpRe - hIm * tmpIm;
      double newIm = hRe * tmpIm + hIm * tmpRe;
      hRe = newRe;
      hIm = newIm;
    }
    return Math.hypot(hRe, hIm);
  }

  private static void expectSectionNear(
      BiquadFilter.Section got, BiquadFilter.Section want, double tol) {
    assertEquals(want.b0, got.b0, tol, "b0");
    assertEquals(want.b1, got.b1, tol, "b1");
    assertEquals(want.b2, got.b2, tol, "b2");
    assertEquals(want.a1, got.a1, tol, "a1");
    assertEquals(want.a2, got.a2, tol, "a2");
  }

  @Test
  void butterworthLowPass4thOrderMatchesScipy() {
    // scipy.signal.butter(4, 50.0, btype='low', fs=1000.0, output='sos')
    BiquadFilter filter = BiquadFilter.butterworth(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0);
    BiquadFilter.Section[] sections = filter.sections();
    assertEquals(2, sections.length);
    expectSectionNear(
        sections[0],
        new BiquadFilter.Section(
            0.00041659920440659937,
            0.0008331984088131987,
            0.00041659920440659937,
            -1.4796742169311934,
            0.5558215432824889),
        1e-10);
    expectSectionNear(
        sections[1],
        new BiquadFilter.Section(1.0, 2.0, 1.0, -1.7009643319435257, 0.7884997398152979),
        1e-10);
  }

  @Test
  void butterworthLowPass8thOrderMatchesScipy() {
    // scipy.signal.butter(8, 100.0, btype='low', fs=1000.0, output='sos')
    BiquadFilter filter = BiquadFilter.butterworth(BiquadFilter.Kind.LowPass, 8, 1000.0, 100.0);
    BiquadFilter.Section[] sections = filter.sections();
    assertEquals(4, sections.length);
    expectSectionNear(
        sections[0],
        new BiquadFilter.Section(
            2.3959644103776166e-05,
            4.791928820755233e-05,
            2.3959644103776166e-05,
            -1.0263514742610553,
            0.26864019099379005),
        1e-10);
    expectSectionNear(
        sections[1],
        new BiquadFilter.Section(1.0, 2.0, 1.0, -1.0868584613628944, 0.343430940165366),
        1e-10);
    expectSectionNear(
        sections[2],
        new BiquadFilter.Section(1.0, 2.0, 1.0, -1.2197253651240232, 0.5076634651740437),
        1e-10);
    expectSectionNear(
        sections[3],
        new BiquadFilter.Section(1.0, 2.0, 1.0, -1.4515795942478362, 0.794251053241888),
        1e-10);
  }

  @Test
  void butterworthLowPassCutoffIsMinusThreeDb() {
    BiquadFilter filter = BiquadFilter.butterworth(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0);
    BiquadFilter.Section[] sections = filter.sections();
    double gainDc = cascadeMagnitude(sections, 0.0, 1000.0);
    double gainFc = cascadeMagnitude(sections, 50.0, 1000.0);
    assertEquals(1.0, gainDc, 1e-10);
    assertEquals(-3.0, 20.0 * Math.log10(gainFc), 0.05);
  }

  @Test
  void butterworthHighPassResponse() {
    BiquadFilter filter = BiquadFilter.butterworth(BiquadFilter.Kind.HighPass, 4, 1000.0, 100.0);
    BiquadFilter.Section[] sections = filter.sections();
    double gainDc = cascadeMagnitude(sections, 0.0, 1000.0);
    double gainFc = cascadeMagnitude(sections, 100.0, 1000.0);
    double gainHigh = cascadeMagnitude(sections, 400.0, 1000.0);
    assertEquals(0.0, gainDc, 1e-10);
    assertEquals(-3.0, 20.0 * Math.log10(gainFc), 0.05);
    assertEquals(1.0, gainHigh, 1e-3);
  }

  @Test
  void butterworthBandPass4thOrderMatchesScipy() {
    // scipy.signal.butter(4, [80.0, 120.0], btype='bandpass', fs=1000.0)
    BiquadFilter filter =
        BiquadFilter.butterworth(BiquadFilter.Kind.BandPass, 4, 1000.0, 80.0, 120.0);
    BiquadFilter.Section[] sections = filter.sections();
    assertEquals(4, sections.length);
    expectSectionNear(
        sections[0],
        new BiquadFilter.Section(
            0.0001832160233696091,
            0.0003664320467392182,
            0.0001832160233696091,
            -1.395944592254935,
            0.7785762494967928),
        1e-10);
    expectSectionNear(
        sections[1],
        new BiquadFilter.Section(1.0, 2.0, 1.0, -1.5194742571654707, 0.8044610397041421),
        1e-10);
    expectSectionNear(
        sections[2],
        new BiquadFilter.Section(1.0, -2.0, 1.0, -1.395095159020637, 0.8950130915917338),
        1e-10);
    expectSectionNear(
        sections[3],
        new BiquadFilter.Section(1.0, -2.0, 1.0, -1.678184355447092, 0.9231164780821922),
        1e-10);
  }

  @Test
  void butterworthBandPassResponse() {
    BiquadFilter filter =
        BiquadFilter.butterworth(BiquadFilter.Kind.BandPass, 4, 1000.0, 80.0, 120.0);
    BiquadFilter.Section[] sections = filter.sections();
    double gainDc = cascadeMagnitude(sections, 0.0, 1000.0);
    double gainCenter = cascadeMagnitude(sections, 100.0, 1000.0);
    double gainNyquist = cascadeMagnitude(sections, 499.0, 1000.0);
    assertTrue(gainDc < 1e-6, "DC gain " + gainDc);
    assertTrue(gainNyquist < 1e-6, "Nyquist gain " + gainNyquist);
    assertTrue(gainCenter > 0.8, "center gain " + gainCenter);
  }

  @Test
  void butterworthBandStopResponse() {
    BiquadFilter filter =
        BiquadFilter.butterworth(BiquadFilter.Kind.BandStop, 4, 1000.0, 80.0, 120.0);
    BiquadFilter.Section[] sections = filter.sections();
    double gainDc = cascadeMagnitude(sections, 0.0, 1000.0);
    double gainCenter = cascadeMagnitude(sections, Math.sqrt(80.0 * 120.0), 1000.0);
    double gainNyquist = cascadeMagnitude(sections, 499.0, 1000.0);
    assertEquals(1.0, gainDc, 1e-3);
    assertEquals(1.0, gainNyquist, 1e-3);
    assertTrue(gainCenter < 1e-6, "center gain " + gainCenter);
  }

  @Test
  void butterworthRejectsInvalidArgs() {
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.butterworth(BiquadFilter.Kind.LowPass, 0, 1000.0, 50.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.butterworth(BiquadFilter.Kind.LowPass, 4, 0.0, 50.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.butterworth(BiquadFilter.Kind.LowPass, 4, 1000.0, 0.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.butterworth(BiquadFilter.Kind.LowPass, 4, 1000.0, 500.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.butterworth(BiquadFilter.Kind.LowPass, 4, 1000.0, 600.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.butterworth(BiquadFilter.Kind.BandPass, 4, 1000.0, 120.0, 80.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.butterworth(BiquadFilter.Kind.BandPass, 4, 1000.0, 80.0, 80.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.butterworth(BiquadFilter.Kind.BandStop, 4, 1000.0, 80.0, 500.0));
  }

  @Test
  void notch60HzMatchesScipy() {
    // scipy.signal.iirnotch(60.0, Q=10.0, fs=1000.0)
    BiquadFilter filter = BiquadFilter.notch(1000.0, 60.0, 10.0);
    BiquadFilter.Section[] sections = filter.sections();
    assertEquals(1, sections.length);
    expectSectionNear(
        sections[0],
        new BiquadFilter.Section(
            0.9814970254751076,
            -1.8251457105120343,
            0.9814970254751076,
            -1.8251457105120341,
            0.9629940509502151),
        1e-12);
  }

  @Test
  void notchAttenuatesAtCenter() {
    BiquadFilter filter = BiquadFilter.notch(1000.0, 60.0, 10.0);
    BiquadFilter.Section[] sections = filter.sections();
    double gainDc = cascadeMagnitude(sections, 0.0, 1000.0);
    double gainNotch = cascadeMagnitude(sections, 60.0, 1000.0);
    double gainFar = cascadeMagnitude(sections, 200.0, 1000.0);
    assertEquals(1.0, gainDc, 1e-6);
    assertTrue(gainNotch < 1e-6, "notch gain " + gainNotch);
    assertEquals(1.0, gainFar, 0.05);
  }

  @Test
  void notchRejectsInvalidArgs() {
    assertThrows(IllegalArgumentException.class, () -> BiquadFilter.notch(0.0, 60.0, 10.0));
    assertThrows(IllegalArgumentException.class, () -> BiquadFilter.notch(-1000.0, 60.0, 10.0));
    assertThrows(IllegalArgumentException.class, () -> BiquadFilter.notch(1000.0, 0.0, 10.0));
    assertThrows(IllegalArgumentException.class, () -> BiquadFilter.notch(1000.0, 500.0, 10.0));
    assertThrows(IllegalArgumentException.class, () -> BiquadFilter.notch(1000.0, 600.0, 10.0));
    assertThrows(IllegalArgumentException.class, () -> BiquadFilter.notch(1000.0, 60.0, 0.0));
    assertThrows(IllegalArgumentException.class, () -> BiquadFilter.notch(1000.0, 60.0, -1.0));
  }

  @Test
  void movingAverageSingleTapIsPassThrough() {
    BiquadFilter filter = BiquadFilter.movingAverage(1);
    BiquadFilter.Section[] sections = filter.sections();
    assertEquals(1, sections.length);
    expectSectionNear(sections[0], new BiquadFilter.Section(1.0, 0.0, 0.0, 0.0, 0.0), 0.0);
  }

  @Test
  void movingAverageEvenLengthHasUnitDcGainAndNyquistNull() {
    BiquadFilter filter = BiquadFilter.movingAverage(4);
    BiquadFilter.Section[] sections = filter.sections();
    double gainDc = cascadeMagnitude(sections, 0.0, 1000.0);
    double gainNyquist = cascadeMagnitude(sections, 500.0, 1000.0);
    assertEquals(1.0, gainDc, 1e-12);
    assertTrue(gainNyquist < 1e-12, "Nyquist gain " + gainNyquist);
  }

  @Test
  void movingAverageOddLengthNullsAtFsOverN() {
    final double fs = 1000.0;
    final int n = 5;
    BiquadFilter filter = BiquadFilter.movingAverage(n);
    BiquadFilter.Section[] sections = filter.sections();
    double gainDc = cascadeMagnitude(sections, 0.0, fs);
    double gainNull = cascadeMagnitude(sections, fs / n, fs);
    double gainHalfNull = cascadeMagnitude(sections, fs / (2.0 * n), fs);
    assertEquals(1.0, gainDc, 1e-12);
    assertTrue(gainNull < 1e-10, "null gain " + gainNull);
    assertTrue(gainHalfNull > 0.1, "half-null gain " + gainHalfNull);
  }

  @Test
  void movingAverageMatchesSumAverageImpulseResponse() {
    final int n = 7;
    BiquadFilter filter = BiquadFilter.movingAverage(n);

    double[] out = new double[n + 3];
    for (int i = 0; i < out.length; i++) {
      double x = i == 0 ? 1.0 : 0.0;
      out[i] = filter.calculate(x);
    }
    for (int i = 0; i < n; i++) {
      assertEquals(1.0 / n, out[i], 1e-12, "tap " + i);
    }
    for (int i = n; i < out.length; i++) {
      assertEquals(0.0, out[i], 1e-12, "post-window " + i);
    }
  }

  @Test
  void movingAverageRejectsInvalidArgs() {
    assertThrows(IllegalArgumentException.class, () -> BiquadFilter.movingAverage(0));
    assertThrows(IllegalArgumentException.class, () -> BiquadFilter.movingAverage(-1));
  }
}
