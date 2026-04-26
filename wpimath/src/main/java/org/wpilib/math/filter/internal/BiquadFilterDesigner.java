// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.filter.internal;

import java.util.ArrayList;
import java.util.List;
import org.wpilib.math.filter.BiquadFilter;
import org.wpilib.math.filter.BiquadFilter.Kind;
import org.wpilib.math.filter.BiquadFilter.Section;

/**
 * Implementation entrypoint for {@link BiquadFilter}'s static design factories. Lives in this
 * sub-package to keep the design machinery (complex arithmetic, Jacobi elliptic functions, ZPK→SOS
 * conversion) out of the public {@code org.wpilib.math.filter} surface.
 *
 * <p>Robot code should call the factories on {@code BiquadFilter} directly — those handle argument
 * validation and wrap the section list in a ready-to-run filter. Calling into this class is
 * supported for niche cases (e.g. inspecting the section list before constructing a filter) but is
 * not part of the documented public API.
 *
 * <p>Each classical-IIR factory (Butterworth, Chebyshev I/II, Elliptic) drives the same three-step
 * pipeline that {@code scipy.signal.iirfilter} does:
 *
 * <ol>
 *   <li>{@link AnalogPrototypes} — analog LP prototype, cutoff 1 rad/s
 *   <li>{@link BilinearTransform#designFromAnalogLp}: kind-specific frequency transform via {@link
 *       Zpk}, bilinear analog→digital at sample rate fs, then ZPK→SOS biquad pairing
 * </ol>
 *
 * <p>SciPy reference for the whole pipeline:
 * https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py (functions {@code
 * iirfilter}, {@code butter}, {@code cheby1}, {@code cheby2}, {@code ellip}).
 *
 * <p>{@code notch} and {@code movingAverage} are closed-form and do not go through the
 * prototype/bilinear path. They are documented inline below.
 */
public final class BiquadFilterDesigner {
  private BiquadFilterDesigner() {}

  private static void validateClassicalArgs(Kind kind, int order, double fs, double f1, double f2) {
    if (order < 1) {
      throw new IllegalArgumentException("BiquadFilter design: order must be >= 1.");
    }
    if (fs <= 0.0) {
      throw new IllegalArgumentException("BiquadFilter design: sample rate must be positive.");
    }
    final double nyquist = 0.5 * fs;
    if (f1 <= 0.0 || f1 >= nyquist) {
      throw new IllegalArgumentException("BiquadFilter design: f1 must lie in (0, fs/2).");
    }
    if ((kind == Kind.BandPass || kind == Kind.BandStop) && (f2 <= f1 || f2 >= nyquist)) {
      throw new IllegalArgumentException(
          "BiquadFilter design: BandPass/BandStop requires f1 < f2 < fs/2.");
    }
  }

  private static void rejectBandKindForLpHpOverload(String factoryName, Kind kind) {
    if (kind == Kind.BandPass || kind == Kind.BandStop) {
      throw new IllegalArgumentException(
          "BiquadFilter."
              + factoryName
              + ": BandPass/BandStop requires f2; use the overload that takes it.");
    }
  }

  /**
   * Designs a Butterworth IIR filter.
   *
   * @see BiquadFilter#butterworth(Kind, int, double, double, double)
   */
  public static Section[] butterworth(Kind kind, int order, double fs, double f1, double f2) {
    validateClassicalArgs(kind, order, fs, f1, f2);
    return toArray(
        BilinearTransform.designFromAnalogLp(
            AnalogPrototypes.butterworthPrototype(order), kind, fs, f1, f2));
  }

  /**
   * Designs a Butterworth IIR low-pass or high-pass filter (single cutoff).
   *
   * @see BiquadFilter#butterworth(Kind, int, double, double)
   */
  public static Section[] butterworth(Kind kind, int order, double fs, double f1) {
    rejectBandKindForLpHpOverload("butterworth", kind);
    return butterworth(kind, order, fs, f1, 0.0);
  }

  /**
   * Designs a Chebyshev Type I IIR filter.
   *
   * @see BiquadFilter#chebyshevI(Kind, int, double, double, double, double)
   */
  public static Section[] chebyshevI(
      Kind kind, int order, double fs, double f1, double f2, double rippleDb) {
    validateClassicalArgs(kind, order, fs, f1, f2);
    if (rippleDb <= 0.0) {
      throw new IllegalArgumentException(
          "BiquadFilter design: ChebyshevI passband ripple must be positive.");
    }
    return toArray(
        BilinearTransform.designFromAnalogLp(
            AnalogPrototypes.chebyshevIPrototype(order, rippleDb), kind, fs, f1, f2));
  }

  /**
   * Designs a Chebyshev Type I IIR low-pass or high-pass filter (single cutoff).
   *
   * @see BiquadFilter#chebyshevI(Kind, int, double, double, double)
   */
  public static Section[] chebyshevI(Kind kind, int order, double fs, double f1, double rippleDb) {
    rejectBandKindForLpHpOverload("chebyshevI", kind);
    return chebyshevI(kind, order, fs, f1, 0.0, rippleDb);
  }

  /**
   * Designs a Chebyshev Type II IIR filter.
   *
   * @see BiquadFilter#chebyshevII(Kind, int, double, double, double, double)
   */
  public static Section[] chebyshevII(
      Kind kind, int order, double fs, double f1, double f2, double stopAttenDb) {
    validateClassicalArgs(kind, order, fs, f1, f2);
    if (stopAttenDb <= 0.0) {
      throw new IllegalArgumentException(
          "BiquadFilter design: ChebyshevII stopband attenuation must be positive.");
    }
    return toArray(
        BilinearTransform.designFromAnalogLp(
            AnalogPrototypes.chebyshevIIPrototype(order, stopAttenDb), kind, fs, f1, f2));
  }

  /**
   * Designs a Chebyshev Type II IIR low-pass or high-pass filter (single cutoff).
   *
   * @see BiquadFilter#chebyshevII(Kind, int, double, double, double)
   */
  public static Section[] chebyshevII(
      Kind kind, int order, double fs, double f1, double stopAttenDb) {
    rejectBandKindForLpHpOverload("chebyshevII", kind);
    return chebyshevII(kind, order, fs, f1, 0.0, stopAttenDb);
  }

  /**
   * Designs an elliptic IIR filter.
   *
   * @see BiquadFilter#elliptic(Kind, int, double, double, double, double, double)
   */
  public static Section[] elliptic(
      Kind kind, int order, double fs, double f1, double f2, double rippleDb, double stopAttenDb) {
    validateClassicalArgs(kind, order, fs, f1, f2);
    if (rippleDb <= 0.0) {
      throw new IllegalArgumentException(
          "BiquadFilter design: Elliptic passband ripple must be positive.");
    }
    if (stopAttenDb <= rippleDb) {
      throw new IllegalArgumentException(
          "BiquadFilter design: Elliptic stopband attenuation must exceed passband ripple.");
    }
    return toArray(
        BilinearTransform.designFromAnalogLp(
            AnalogPrototypes.ellipticPrototype(order, rippleDb, stopAttenDb), kind, fs, f1, f2));
  }

  /**
   * Designs an elliptic IIR low-pass or high-pass filter (single cutoff).
   *
   * @see BiquadFilter#elliptic(Kind, int, double, double, double, double)
   */
  public static Section[] elliptic(
      Kind kind, int order, double fs, double f1, double rippleDb, double stopAttenDb) {
    rejectBandKindForLpHpOverload("elliptic", kind);
    return elliptic(kind, order, fs, f1, 0.0, rippleDb, stopAttenDb);
  }

  /**
   * Designs a notch (band-stop) IIR filter.
   *
   * @see BiquadFilter#notch(double, double, double)
   */
  public static Section[] notch(double fs, double f0, double q) {
    if (fs <= 0.0) {
      throw new IllegalArgumentException("BiquadFilter.notch: sample rate must be positive.");
    }
    if (q <= 0.0) {
      throw new IllegalArgumentException("BiquadFilter.notch: quality factor must be positive.");
    }
    final double nyquist = 0.5 * fs;
    if (f0 <= 0.0 || f0 >= nyquist) {
      throw new IllegalArgumentException("BiquadFilter.notch: f0 must lie in (0, fs/2).");
    }

    // Standard second-order IIR notch (zero pair on the unit circle at ±w0,
    // pole pair just inside on the same radial line). Matches
    // scipy.signal.iirnotch(f0, Q, fs) exactly:
    //   w0 = 2π·f0/fs,  bw = w0/Q,  β = tan(bw/2),  g = 1/(1 + β)
    //   b = g · [1, -2cos(w0), 1],  a = [1, -2g·cos(w0), 2g - 1]
    // SciPy reference (function iirnotch):
    //   https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py
    // Background: Sophocles Orfanidis, "Introduction to Signal Processing"
    // §11.3.2 ("Parametric resonators and notch filters").
    final double w0 = 2.0 * Math.PI * f0 / fs;
    final double bw = w0 / q;
    final double beta = Math.tan(0.5 * bw);
    final double gain = 1.0 / (1.0 + beta);
    final double cosW0 = Math.cos(w0);
    return new Section[] {
      new Section(gain, -2.0 * gain * cosW0, gain, -2.0 * gain * cosW0, 2.0 * gain - 1.0)
    };
  }

  /**
   * Designs a moving-average FIR filter expressed as cascaded biquad sections.
   *
   * @see BiquadFilter#movingAverage(int)
   */
  public static Section[] movingAverage(int taps) {
    if (taps < 1) {
      throw new IllegalArgumentException("BiquadFilter.movingAverage: taps must be >= 1.");
    }
    // A length-N moving average has H(z) = (1/N)·(1 - z⁻ᴺ)/(1 - z⁻¹), whose
    // non-trivial zeros are the Nth roots of unity except z = 1:
    //   z_k = exp(i·2π·k/N), k = 1..N-1
    // Pair each (z_k, z_{N-k}) into one all-zero biquad
    //   (b0, b1, b2, a1, a2) = (1, -2·cos(2πk/N), 1, 0, 0)
    // and, if N is even, emit a single-zero first-order biquad for the unpaired
    // root at z = -1:
    //   (1, 1, 0, 0, 0)
    // The overall 1/N gain is folded into the first section.
    //
    // The factorization of (1 - z⁻ᴺ) into roots of unity is textbook:
    //   https://en.wikipedia.org/wiki/Root_of_unity#Polynomial_form
    // Equivalent to scipy.signal.tf2sos applied to b = [1/N, ..., 1/N], a = [1].
    if (taps == 1) {
      return new Section[] {new Section(1.0, 0.0, 0.0, 0.0, 0.0)};
    }
    final double N = taps;
    final int pairs = (taps - 1) / 2;
    List<Section> out = new ArrayList<>();
    for (int k = 1; k <= pairs; k++) {
      double c = Math.cos(2.0 * Math.PI * k / N);
      out.add(new Section(1.0, -2.0 * c, 1.0, 0.0, 0.0));
    }
    if (taps % 2 == 0) {
      out.add(new Section(1.0, 1.0, 0.0, 0.0, 0.0));
    }
    final double g = 1.0 / N;
    Section first = out.get(0);
    out.set(0, new Section(first.b0 * g, first.b1 * g, first.b2 * g, first.a1, first.a2));
    return out.toArray(new Section[0]);
  }

  private static Section[] toArray(List<Section> list) {
    return list.toArray(new Section[0]);
  }
}
