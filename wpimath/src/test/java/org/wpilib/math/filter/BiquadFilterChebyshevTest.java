// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.filter;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class BiquadFilterChebyshevTest {
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

  // ----- Chebyshev type I --------------------------------------------------

  @Test
  void cheby1LowPass4thOrderMatchesScipy() {
    // scipy.signal.cheby1(4, 1.0, 50.0, btype='low', fs=1000.0, output='sos')
    BiquadFilter filter = BiquadFilter.chebyshevI(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, 1.0);
    BiquadFilter.Section[] sections = filter.sections();
    assertEquals(2, sections.length);
    expectSectionNear(
        sections[0],
        new BiquadFilter.Section(
            0.00012984963538691335,
            0.0002596992707738267,
            0.00012984963538691335,
            -1.7831991339963722,
            0.8083720161261031),
        1e-10);
    expectSectionNear(
        sections[1],
        new BiquadFilter.Section(1.0, 2.0, 1.0, -1.8246970351326663, 0.917300614770565),
        1e-10);
  }

  @Test
  void cheby1HighPass4thOrderMatchesScipy() {
    // scipy.signal.cheby1(4, 1.0, 100.0, btype='high', fs=1000.0, output='sos')
    BiquadFilter filter =
        BiquadFilter.chebyshevI(BiquadFilter.Kind.HighPass, 4, 1000.0, 100.0, 1.0);
    BiquadFilter.Section[] sections = filter.sections();
    assertEquals(2, sections.length);
    expectSectionNear(
        sections[0],
        new BiquadFilter.Section(
            0.3439348735216468,
            -0.6878697470432936,
            0.3439348735216468,
            -0.5756927885601547,
            0.2749869650540311),
        1e-10);
    expectSectionNear(
        sections[1],
        new BiquadFilter.Section(1.0, -2.0, 1.0, -1.4896289697923346, 0.8466697013585162),
        1e-10);
  }

  @Test
  void cheby1BandPass4thOrderMatchesScipy() {
    // scipy.signal.cheby1(4, 1.0, [80.0, 120.0], btype='bandpass', fs=1000.0)
    BiquadFilter filter =
        BiquadFilter.chebyshevI(BiquadFilter.Kind.BandPass, 4, 1000.0, 80.0, 120.0, 1.0);
    BiquadFilter.Section[] sections = filter.sections();
    assertEquals(4, sections.length);
    expectSectionNear(
        sections[0],
        new BiquadFilter.Section(
            5.463638752463053e-05,
            0.00010927277504926106,
            5.463638752463053e-05,
            -1.4985467271298947,
            0.9129301418072939),
        1e-10);
    expectSectionNear(
        sections[1],
        new BiquadFilter.Section(1.0, 2.0, 1.0, -1.6224939133759921, 0.9242414431352561),
        1e-10);
    expectSectionNear(
        sections[2],
        new BiquadFilter.Section(1.0, -2.0, 1.0, -1.4320495577056345, 0.9601480937923097),
        1e-10);
    expectSectionNear(
        sections[3],
        new BiquadFilter.Section(1.0, -2.0, 1.0, -1.7261705273848356, 0.9716328706093393),
        1e-10);
  }

  @Test
  void cheby1LowPassPassbandStaysWithinRipple() {
    final double rippleDb = 1.0;
    BiquadFilter filter =
        BiquadFilter.chebyshevI(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, rippleDb);
    BiquadFilter.Section[] sections = filter.sections();

    // For even order, |H(0)| = 1/sqrt(1+eps^2) — i.e. -ripple dB at DC.
    double dcDb = 20.0 * Math.log10(cascadeMagnitude(sections, 0.0, 1000.0));
    assertEquals(-rippleDb, dcDb, 0.01);

    // |H(fc)| = 1/sqrt(1+eps^2) too (ripple boundary).
    double fcDb = 20.0 * Math.log10(cascadeMagnitude(sections, 50.0, 1000.0));
    assertEquals(-rippleDb, fcDb, 0.01);

    // Strong attenuation past the cutoff.
    double stopDb = 20.0 * Math.log10(cascadeMagnitude(sections, 200.0, 1000.0));
    assertTrue(stopDb < -40.0, "stopband " + stopDb);
  }

  @Test
  void cheby1OddOrderHasUnityDcGain() {
    BiquadFilter filter = BiquadFilter.chebyshevI(BiquadFilter.Kind.LowPass, 5, 1000.0, 50.0, 1.0);
    double gainDc = cascadeMagnitude(filter.sections(), 0.0, 1000.0);
    assertEquals(1.0, gainDc, 1e-9);
  }

  @Test
  void cheby1RejectsInvalidArgs() {
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.chebyshevI(BiquadFilter.Kind.LowPass, 0, 1000.0, 50.0, 1.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.chebyshevI(BiquadFilter.Kind.LowPass, 4, 0.0, 50.0, 1.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.chebyshevI(BiquadFilter.Kind.LowPass, 4, 1000.0, 0.0, 1.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.chebyshevI(BiquadFilter.Kind.LowPass, 4, 1000.0, 600.0, 1.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.chebyshevI(BiquadFilter.Kind.BandPass, 4, 1000.0, 120.0, 80.0, 1.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.chebyshevI(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, 0.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.chebyshevI(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, -1.0));
  }

  // ----- Chebyshev type II -------------------------------------------------

  @Test
  void cheby2LowPass4thOrderMatchesScipy() {
    // scipy.signal.cheby2(4, 40.0, 50.0, btype='low', fs=1000.0, output='sos')
    BiquadFilter filter =
        BiquadFilter.chebyshevII(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, 40.0);
    BiquadFilter.Section[] sections = filter.sections();
    assertEquals(2, sections.length);
    expectSectionNear(
        sections[0],
        new BiquadFilter.Section(
            0.009735570656077937,
            -0.01377605024474192,
            0.009735570656077937,
            -1.6993957730842835,
            0.7262535657383176),
        1e-10);
    expectSectionNear(
        sections[1],
        new BiquadFilter.Section(
            1.0, -1.8857977835164716, 1.0, -1.87354703561714, 0.8977631739778823),
        1e-10);
  }

  @Test
  void cheby2HighPassResponse() {
    // For HP/BS, zero pairings can differ from scipy without affecting the
    // cascade response. Verify the response at points that uniquely
    // characterize the filter rather than per-section coefficients.
    final double attenDb = 40.0;
    BiquadFilter filter =
        BiquadFilter.chebyshevII(BiquadFilter.Kind.HighPass, 4, 1000.0, 100.0, attenDb);
    BiquadFilter.Section[] sections = filter.sections();

    double gainHigh = cascadeMagnitude(sections, 400.0, 1000.0);
    assertEquals(1.0, gainHigh, 1e-3);

    double fcDb = 20.0 * Math.log10(cascadeMagnitude(sections, 100.0, 1000.0));
    assertTrue(fcDb < -attenDb + 0.01, "fc " + fcDb);

    double dcDb = 20.0 * Math.log10(cascadeMagnitude(sections, 0.0, 1000.0));
    assertTrue(dcDb < -attenDb + 0.5, "DC " + dcDb);
  }

  @Test
  void cheby2BandStopResponse() {
    final double attenDb = 40.0;
    BiquadFilter filter =
        BiquadFilter.chebyshevII(BiquadFilter.Kind.BandStop, 4, 1000.0, 80.0, 120.0, attenDb);
    BiquadFilter.Section[] sections = filter.sections();

    assertEquals(1.0, cascadeMagnitude(sections, 0.0, 1000.0), 1e-3);
    assertEquals(1.0, cascadeMagnitude(sections, 400.0, 1000.0), 1e-3);

    double lowEdgeDb = 20.0 * Math.log10(cascadeMagnitude(sections, 80.0, 1000.0));
    double highEdgeDb = 20.0 * Math.log10(cascadeMagnitude(sections, 120.0, 1000.0));
    assertTrue(lowEdgeDb < -attenDb + 0.01, "low edge " + lowEdgeDb);
    assertTrue(highEdgeDb < -attenDb + 0.01, "high edge " + highEdgeDb);
  }

  @Test
  void cheby2LowPassFlatPassbandRipplesInStopband() {
    final double attenDb = 40.0;
    BiquadFilter filter =
        BiquadFilter.chebyshevII(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, attenDb);
    BiquadFilter.Section[] sections = filter.sections();

    // Cheby2 has |H(0)| = 1 always (no DC ripple).
    assertEquals(1.0, cascadeMagnitude(sections, 0.0, 1000.0), 1e-6);

    double fcDb = 20.0 * Math.log10(cascadeMagnitude(sections, 50.0, 1000.0));
    assertTrue(fcDb < -attenDb + 0.01, "fc " + fcDb);

    double deepDb = 20.0 * Math.log10(cascadeMagnitude(sections, 100.0, 1000.0));
    assertTrue(deepDb < -attenDb + 0.5, "deep " + deepDb);
  }

  @Test
  void cheby2RejectsInvalidArgs() {
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.chebyshevII(BiquadFilter.Kind.LowPass, 0, 1000.0, 50.0, 40.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.chebyshevII(BiquadFilter.Kind.LowPass, 4, 0.0, 50.0, 40.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.chebyshevII(BiquadFilter.Kind.LowPass, 4, 1000.0, 0.0, 40.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.chebyshevII(BiquadFilter.Kind.LowPass, 4, 1000.0, 600.0, 40.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.chebyshevII(BiquadFilter.Kind.BandPass, 4, 1000.0, 120.0, 80.0, 40.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.chebyshevII(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, 0.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.chebyshevII(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, -10.0));
  }
}
