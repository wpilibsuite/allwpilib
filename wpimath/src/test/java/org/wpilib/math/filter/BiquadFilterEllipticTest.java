// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.filter;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class BiquadFilterEllipticTest {
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
  void lowPass4thOrderMatchesScipy() {
    // scipy.signal.ellip(4, 1.0, 40.0, 50.0, btype='low', fs=1000.0)
    BiquadFilter filter =
        BiquadFilter.elliptic(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, 1.0, 40.0);
    BiquadFilter.Section[] sections = filter.sections();
    assertEquals(2, sections.length);
    expectSectionNear(
        sections[0],
        new BiquadFilter.Section(
            0.011738158079014929,
            -0.01231742214386518,
            0.011738158079014929,
            -1.7624726990429698,
            0.7947551993829407),
        1e-10);
    expectSectionNear(
        sections[1],
        new BiquadFilter.Section(
            1.0, -1.7559103274197139, 1.0, -1.8423125689214854, 0.9369806105943849),
        1e-10);
  }

  @Test
  void lowPassOddOrder5HasFirstOrderSection() {
    // Odd order means one first-order-as-biquad section in the cascade (a2 = 0
    // and b2 = 0 for that section). scipy emits 3 sections — verify count and
    // shape rather than coefficient-by-coefficient, because section ordering
    // and zero pairing have the same scipy-vs-ours freedom as Butterworth BP.
    BiquadFilter filter =
        BiquadFilter.elliptic(BiquadFilter.Kind.LowPass, 5, 1000.0, 50.0, 1.0, 40.0);
    BiquadFilter.Section[] sections = filter.sections();
    assertEquals(3, sections.length);

    int firstOrder = 0;
    for (BiquadFilter.Section s : sections) {
      if (s.a2 == 0.0 && s.b2 == 0.0) {
        firstOrder++;
      }
    }
    assertEquals(1, firstOrder);
  }

  @Test
  void lowPassEquirippleInPassbandAndStopband() {
    final double ripple = 1.0;
    final double atten = 40.0;
    BiquadFilter filter =
        BiquadFilter.elliptic(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, ripple, atten);
    BiquadFilter.Section[] sections = filter.sections();

    double dcDb = 20.0 * Math.log10(cascadeMagnitude(sections, 0.0, 1000.0));
    double fcDb = 20.0 * Math.log10(cascadeMagnitude(sections, 50.0, 1000.0));
    assertEquals(-ripple, dcDb, 0.01);
    assertEquals(-ripple, fcDb, 0.01);

    double stopDb = 20.0 * Math.log10(cascadeMagnitude(sections, 100.0, 1000.0));
    assertTrue(stopDb < -atten + 0.5, "stop " + stopDb);
  }

  @Test
  void oddOrderHasUnityDcGain() {
    BiquadFilter filter =
        BiquadFilter.elliptic(BiquadFilter.Kind.LowPass, 5, 1000.0, 50.0, 1.0, 40.0);
    double gainDc = cascadeMagnitude(filter.sections(), 0.0, 1000.0);
    assertEquals(1.0, gainDc, 1e-9);
  }

  @Test
  void highPassResponse() {
    final double ripple = 1.0;
    final double atten = 40.0;
    BiquadFilter filter =
        BiquadFilter.elliptic(BiquadFilter.Kind.HighPass, 4, 1000.0, 100.0, ripple, atten);
    BiquadFilter.Section[] sections = filter.sections();

    double passbandDb = 20.0 * Math.log10(cascadeMagnitude(sections, 400.0, 1000.0));
    assertEquals(0.0, passbandDb, ripple + 0.01);

    double cutoffDb = 20.0 * Math.log10(cascadeMagnitude(sections, 100.0, 1000.0));
    assertEquals(-ripple, cutoffDb, 0.01);

    double dcDb = 20.0 * Math.log10(cascadeMagnitude(sections, 0.0, 1000.0));
    assertTrue(dcDb < -atten + 0.5, "DC " + dcDb);
  }

  @Test
  void bandPass4thOrderMatchesScipy() {
    // scipy.signal.ellip(4, 1.0, 40.0, [80.0, 120.0], btype='bandpass', fs=1000.0)
    BiquadFilter filter =
        BiquadFilter.elliptic(BiquadFilter.Kind.BandPass, 4, 1000.0, 80.0, 120.0, 1.0, 40.0);
    BiquadFilter.Section[] sections = filter.sections();
    assertEquals(4, sections.length);
    expectSectionNear(
        sections[0],
        new BiquadFilter.Section(
            0.010903156756394984,
            -0.008920205787636758,
            0.010903156756394982,
            -1.4809043488404827,
            0.9052184223450329),
        1e-10);
    expectSectionNear(
        sections[1],
        new BiquadFilter.Section(
            1.0, -1.9038045463534676, 0.9999999999999999, -1.62699499510272, 0.9194678402475894),
        1e-10);
    expectSectionNear(
        sections[2],
        new BiquadFilter.Section(
            1.0, -1.3265553048553793, 1.0, -1.4370735618061194, 0.9697500844409095),
        1e-10);
    expectSectionNear(
        sections[3],
        new BiquadFilter.Section(
            1.0, -1.8057300347135379, 0.9999999999999998, -1.733243724674222, 0.978571861817194),
        1e-10);
  }

  @Test
  void bandPassResponse() {
    final double ripple = 1.0;
    final double atten = 40.0;
    BiquadFilter filter =
        BiquadFilter.elliptic(BiquadFilter.Kind.BandPass, 3, 1000.0, 80.0, 120.0, ripple, atten);
    BiquadFilter.Section[] sections = filter.sections();

    double centerDb = 20.0 * Math.log10(cascadeMagnitude(sections, 100.0, 1000.0));
    assertEquals(0.0, centerDb, ripple + 0.01);

    double belowDb = 20.0 * Math.log10(cascadeMagnitude(sections, 0.0, 1000.0));
    double aboveDb = 20.0 * Math.log10(cascadeMagnitude(sections, 400.0, 1000.0));
    assertTrue(belowDb < -atten + 1.0, "below " + belowDb);
    assertTrue(aboveDb < -atten + 1.0, "above " + aboveDb);
  }

  @Test
  void rejectsInvalidArgs() {
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.elliptic(BiquadFilter.Kind.LowPass, 0, 1000.0, 50.0, 1.0, 40.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.elliptic(BiquadFilter.Kind.LowPass, 4, 0.0, 50.0, 1.0, 40.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.elliptic(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, 0.0, 40.0));
    // Stopband must be deeper than passband ripple.
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.elliptic(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, 40.0, 1.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.elliptic(BiquadFilter.Kind.LowPass, 4, 1000.0, 50.0, 5.0, 5.0));
    // Frequencies out of range / inverted.
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.elliptic(BiquadFilter.Kind.LowPass, 4, 1000.0, 600.0, 1.0, 40.0));
    assertThrows(
        IllegalArgumentException.class,
        () -> BiquadFilter.elliptic(BiquadFilter.Kind.BandPass, 4, 1000.0, 120.0, 80.0, 1.0, 40.0));
  }
}
