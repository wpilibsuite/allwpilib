// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.filter.internal;

import java.util.List;
import org.wpilib.math.filter.BiquadFilter;

/**
 * Bilinear transform plus the kind-specific dispatch shared by every classical IIR factory.
 *
 * <p>Standard analog→digital conversion: pre-warp the digital cutoff so the post-bilinear digital
 * response hits the requested edge exactly, then map each analog pole p (resp. zero z) via s →
 * 2·fs·(z-1)/(z+1), giving p_d = (2fs + p) / (2fs - p).
 *
 * <p>Background:
 *
 * <ul>
 *   <li>https://en.wikipedia.org/wiki/Bilinear_transform
 *   <li>Oppenheim &amp; Schafer, "Discrete-Time Signal Processing" §7.2.2
 * </ul>
 *
 * <p>SciPy implementations to compare against, line for line, in
 * https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py — functions {@code
 * bilinear_zpk} and {@code _zpkbilinear}; constant prewarping is folded into the {@code
 * lp2{lp,hp,bp,bs}_zpk} callers above the bilinear step.
 */
final class BilinearTransform {
  private BilinearTransform() {}

  /**
   * Pre-warp a digital cutoff frequency (Hz) for use as the analog-domain cutoff that, after the
   * bilinear transform at the same {@code fs}, maps back to exactly that digital cutoff.
   */
  static double preWarp(double fc, double fs) {
    return 2.0 * fs * Math.tan(Math.PI * fc / fs);
  }

  /**
   * Bilinear transform of an analog ZPK to a digital ZPK at sample rate {@code fs}. Analog zeros at
   * infinity map to digital zeros at z = -1 (Nyquist).
   */
  static Zpk bilinearTransform(Zpk analog, double fs) {
    // Substituting s = 2fs(z-1)/(z+1) into H(s) and solving for the image of
    // each finite root gives p_d = (2fs + p)/(2fs - p) (and the same form for
    // zeros). The substitution also rescales the leading polynomial coefficient
    // by Π(2fs - z) / Π(2fs - p) over the analog roots — that's the gain
    // adjustment at the bottom.
    Zpk out = new Zpk();
    double fs2 = 2.0 * fs;
    Complex zNumProd = Complex.ONE;
    Complex zDenProd = Complex.ONE;
    for (Complex z : analog.zeros) {
      Complex denom = new Complex(fs2, 0).sub(z);
      out.zeros.add(new Complex(fs2, 0).add(z).div(denom));
      zNumProd = zNumProd.mul(denom);
    }
    for (Complex p : analog.poles) {
      Complex denom = new Complex(fs2, 0).sub(p);
      out.poles.add(new Complex(fs2, 0).add(p).div(denom));
      zDenProd = zDenProd.mul(denom);
    }
    // Analog filters with fewer zeros than poles have `degree` zeros at s=∞.
    // The bilinear maps s=∞ to z=-1 (Nyquist), so materialize them here. This
    // is what gives a Butterworth low-pass its N digital zeros at Nyquist and
    // hence its hard rolloff at the top of the band.
    int degree = Zpk.relativeDegree(analog);
    for (int i = 0; i < degree; i++) {
      out.zeros.add(new Complex(-1.0, 0.0));
    }
    out.gain = analog.gain * zNumProd.div(zDenProd).real();
    return out;
  }

  /**
   * Apply the kind-specific frequency transform (LP/HP/BP/BS) to an analog LP prototype, run the
   * bilinear transform at {@code fs}, and convert to a SOS cascade. Shared by every classical IIR
   * design factory (Butterworth, Chebyshev I/II, Elliptic).
   *
   * <p>Caller is responsible for validating inputs (positive fs, f1 in (0, fs/2), and for BP/BS, f1
   * &lt; f2 &lt; fs/2). This helper does no validation itself.
   */
  static List<BiquadFilter.Section> designFromAnalogLp(
      Zpk analogLp, BiquadFilter.Kind kind, double fs, double f1, double f2) {
    // Pipeline:
    //   1. Pre-warp the requested digital cutoff(s) into the analog cutoff
    //      that maps back to them under the bilinear transform.
    //   2. Reshape the 1 rad/s LP prototype with a kind-specific s-plane
    //      substitution (LP→LP/HP/BP/BS), giving an analog filter at the
    //      requested kind and cutoff.
    //   3. Bilinear-transform the resulting analog ZPK to a digital ZPK
    //      (s-plane → z-plane).
    //   4. Pair conjugate digital roots into a cascade of real-coefficient
    //      biquad sections.
    Zpk analog = analogLp;
    switch (kind) {
      case LowPass:
        analog = Zpk.analogLpToLp(analog, preWarp(f1, fs));
        break;
      case HighPass:
        analog = Zpk.analogLpToHp(analog, preWarp(f1, fs));
        break;
      case BandPass:
        {
          double w1 = preWarp(f1, fs);
          double w2 = preWarp(f2, fs);
          double wo = Math.sqrt(w1 * w2);
          double bw = w2 - w1;
          analog = Zpk.analogLpToBp(analog, wo, bw);
          break;
        }
      case BandStop:
        {
          double w1 = preWarp(f1, fs);
          double w2 = preWarp(f2, fs);
          double wo = Math.sqrt(w1 * w2);
          double bw = w2 - w1;
          analog = Zpk.analogLpToBs(analog, wo, bw);
          break;
        }
      default:
        throw new IllegalArgumentException("Unknown BiquadFilter.Kind: " + kind);
    }
    return Zpk.zpkToSos(bilinearTransform(analog, fs));
  }
}
