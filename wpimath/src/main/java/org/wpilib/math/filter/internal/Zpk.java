// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.filter.internal;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import org.wpilib.math.filter.BiquadFilter;

/**
 * Zeros/poles/gain representation of a rational transfer function.
 *
 * <pre>
 *   H(s) = gain · ∏(s - z_i) / ∏(s - p_j)        (analog)
 *   H(z) = gain · ∏(z - z_i) / ∏(z - p_j)        (digital)
 * </pre>
 *
 * <p>Complex roots must appear in conjugate pairs; that invariant is preserved by every transform
 * below.
 *
 * <p>The four {@code analogLpTo*} helpers are the standard frequency-domain spectral
 * transformations (Oppenheim &amp; Schafer, "Discrete-Time Signal Processing" §7.1.5;
 * Constantinides, "Spectral transformations for digital filters", IEE Proc. 117 (1970) 1585–1590).
 * They each correspond to a SciPy helper:
 *
 * <ul>
 *   <li>{@code analogLpToLp} ↔ {@code scipy.signal.lp2lp_zpk}
 *   <li>{@code analogLpToHp} ↔ {@code scipy.signal.lp2hp_zpk}
 *   <li>{@code analogLpToBp} ↔ {@code scipy.signal.lp2bp_zpk}
 *   <li>{@code analogLpToBs} ↔ {@code scipy.signal.lp2bs_zpk}
 * </ul>
 *
 * <p>Source for all four: https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py
 *
 * <p>{@code zpkToSos} pairs conjugate roots into biquad sections using the same "nearest pole/zero"
 * pairing that {@code scipy.signal.zpk2sos} uses by default (helper {@code _cplxreal}). We diverge
 * from scipy in only one place: section ordering. SciPy can return a "minimum phase" ordering; we
 * always sort by ascending |pole| (least aggressive first). The cascade product is identical; only
 * the per-section numerical conditioning differs.
 */
final class Zpk {
  final List<Complex> zeros = new ArrayList<>();
  final List<Complex> poles = new ArrayList<>();
  double gain = 1.0;

  // A root is treated as real when |imag| falls below this. The same tolerance
  // is used to match conjugates, since after bilinear/LP→BP transforms the real
  // and imaginary drift of a true pair is of the same order.
  private static final double IMAG_TOLERANCE = 1e-10;

  private static final class Partitioned {
    final List<Complex> complexPairs = new ArrayList<>();
    final List<Double> realRoots = new ArrayList<>();
  }

  // Partition a (conjugate-symmetric) root list into a vector of complex roots
  // represented by the upper-half-plane conjugate-pair representative, plus a
  // vector of real roots.
  private static Partitioned partition(List<Complex> roots) {
    Partitioned out = new Partitioned();
    boolean[] matched = new boolean[roots.size()];
    for (int i = 0; i < roots.size(); i++) {
      if (matched[i]) {
        continue;
      }
      matched[i] = true;
      Complex r = roots.get(i);
      if (Math.abs(r.imag()) < IMAG_TOLERANCE) {
        out.realRoots.add(r.real());
        continue;
      }
      Complex rep = r.imag() > 0 ? r : r.conj();
      // Find unmatched conjugate in the remaining list. Callers pass
      // conjugate-symmetric inputs; if no partner is found the input violated
      // that invariant (or drifted numerically past IMAG_TOLERANCE), and the
      // cascade that follows would silently double-count the orphan.
      boolean found = false;
      for (int j = i + 1; j < roots.size(); j++) {
        if (matched[j]) {
          continue;
        }
        Complex rj = roots.get(j);
        if (Math.abs(rj.imag() + r.imag()) < IMAG_TOLERANCE
            && Math.abs(rj.real() - r.real()) < IMAG_TOLERANCE) {
          matched[j] = true;
          found = true;
          break;
        }
      }
      if (!found) {
        throw new IllegalStateException("Zpk root list is not conjugate-symmetric");
      }
      out.complexPairs.add(rep);
    }
    return out;
  }

  static int relativeDegree(Zpk p) {
    return p.poles.size() - p.zeros.size();
  }

  // The underlying LP→BP/BS substitutions are s → (s² + wo²)/(bw·s) for BP and
  // the reciprocal for BS. Plugging either into a prototype factor (s - r) and
  // clearing denominators yields a quadratic in s whose two roots become a
  // conjugate pair around ±j·wo. Specifically:
  //   BP:  s² - bw·r·s + wo² = 0
  //   BS:  s² - (bw/r)·s + wo² = 0
  // The caller folds the family-specific scaling into rScaled (bw·r/2 for BP,
  // bw/(2·r) for BS) so this helper just solves the unified quadratic
  //   s² - 2·rScaled·s + wo² = 0  →  rScaled ± sqrt(rScaled² - wo²).
  private static Complex[] bpRoots(Complex rScaled, double wo) {
    Complex disc = rScaled.mul(rScaled).sub(wo * wo).sqrt();
    return new Complex[] {rScaled.add(disc), rScaled.sub(disc)};
  }

  /** Analog LP→LP transform: cutoff 1 rad/s → cutoff {@code wo} rad/s. */
  static Zpk analogLpToLp(Zpk p, double wo) {
    Zpk out = new Zpk();
    out.gain = p.gain;
    for (Complex z : p.zeros) {
      out.zeros.add(z.mul(wo));
    }
    for (Complex pole : p.poles) {
      out.poles.add(pole.mul(wo));
    }
    out.gain *= Math.pow(wo, relativeDegree(p));
    return out;
  }

  /** Analog LP→HP transform: LP cutoff 1 → HP cutoff {@code wo} rad/s. */
  static Zpk analogLpToHp(Zpk p, double wo) {
    // Mirror: s → wo/s. Finite zeros/poles invert and scale; zeros at infinity
    // become zeros at the origin to balance the pole count.
    Zpk out = new Zpk();
    Complex zProd = Complex.ONE;
    Complex pProd = Complex.ONE;
    for (Complex z : p.zeros) {
      out.zeros.add(new Complex(wo, 0).div(z));
      zProd = zProd.mul(z.negate());
    }
    for (Complex pole : p.poles) {
      out.poles.add(new Complex(wo, 0).div(pole));
      pProd = pProd.mul(pole.negate());
    }
    int degree = relativeDegree(p);
    for (int i = 0; i < degree; i++) {
      out.zeros.add(new Complex(0.0, 0.0));
    }
    out.gain = p.gain * zProd.div(pProd).real();
    return out;
  }

  /**
   * Analog LP→BP transform centered at {@code wo} rad/s with bandwidth {@code bw} rad/s. Each
   * prototype pole becomes two; each prototype zero becomes two; plus {@code degree} zeros at the
   * origin.
   */
  static Zpk analogLpToBp(Zpk p, double wo, double bw) {
    Zpk out = new Zpk();
    for (Complex z : p.zeros) {
      Complex[] zs = bpRoots(z.mul(bw * 0.5), wo);
      out.zeros.add(zs[0]);
      out.zeros.add(zs[1]);
    }
    for (Complex pole : p.poles) {
      Complex[] ps = bpRoots(pole.mul(bw * 0.5), wo);
      out.poles.add(ps[0]);
      out.poles.add(ps[1]);
    }
    int degree = relativeDegree(p);
    for (int i = 0; i < degree; i++) {
      out.zeros.add(new Complex(0.0, 0.0));
    }
    out.gain = p.gain * Math.pow(bw, degree);
    return out;
  }

  /**
   * Analog LP→BS transform centered at {@code wo} rad/s with bandwidth {@code bw} rad/s. Same
   * fan-out as LpToBp; the added zeros go to ±j·wo instead of the origin.
   */
  static Zpk analogLpToBs(Zpk p, double wo, double bw) {
    Zpk out = new Zpk();
    Complex zProd = Complex.ONE;
    Complex pProd = Complex.ONE;
    Complex halfBw = new Complex(bw * 0.5, 0);
    for (Complex z : p.zeros) {
      Complex[] zs = bpRoots(halfBw.div(z), wo);
      out.zeros.add(zs[0]);
      out.zeros.add(zs[1]);
      zProd = zProd.mul(z.negate());
    }
    for (Complex pole : p.poles) {
      Complex[] ps = bpRoots(halfBw.div(pole), wo);
      out.poles.add(ps[0]);
      out.poles.add(ps[1]);
      pProd = pProd.mul(pole.negate());
    }
    int degree = relativeDegree(p);
    Complex jwo = new Complex(0.0, wo);
    for (int i = 0; i < degree; i++) {
      out.zeros.add(jwo);
      out.zeros.add(jwo.negate());
    }
    out.gain = p.gain * zProd.div(pProd).real();
    return out;
  }

  /**
   * Pair conjugate poles (and zeros) into biquad sections. Sections are ordered by ascending |pole|
   * (innermost / least-aggressive first); the overall gain is folded into the first section's
   * numerator.
   */
  static List<BiquadFilter.Section> zpkToSos(Zpk digital) {
    // A conjugate pair (p, p̄) factors to (z - p)(z - p̄) = z² - 2·Re(p)·z + |p|²,
    // a real-coefficient quadratic — that's how complex roots become the real
    // (b0,b1,b2) and (1,a1,a2) the runtime needs. Same identity for zero pairs.
    //
    // Below: partition roots into complex pairs + lone reals, sort poles by
    // |pole| (least aggressive first, for numerical conditioning), pair each
    // pole pair with its nearest zero pair (scipy's "nearest" rule), and emit
    // one biquad per pole pair (or per real pole for odd order). Leftover real
    // zeros fill in the remaining biquad numerators.
    Partitioned polePart = partition(digital.poles);
    Partitioned zeroPart = partition(digital.zeros);

    // Least-aggressive (smallest |pole|) sections go first, so scipy-style
    // golden values line up and the numerically tightest biquad sits last.
    polePart.complexPairs.sort(Comparator.comparingDouble(Complex::normSq));

    // Pre-assign complex zeros to complex poles using scipy's 'nearest' pairing:
    // process from worst pole (largest |p|, last in ascending sort) to best,
    // each picking the nearest unused complex zero by Euclidean distance.
    // This is deterministic even when all zeros have equal magnitude (e.g.
    // Chebyshev II LP where every digital zero sits on the unit circle).
    int numCplxPoles = polePart.complexPairs.size();
    Complex[] cplxZeroForPole = new Complex[numCplxPoles];
    boolean[] hasCplxZero = new boolean[numCplxPoles];
    for (int i = numCplxPoles - 1; i >= 0 && !zeroPart.complexPairs.isEmpty(); i--) {
      final Complex p = polePart.complexPairs.get(i);
      int bestIdx = 0;
      double bestDist = Double.POSITIVE_INFINITY;
      for (int j = 0; j < zeroPart.complexPairs.size(); j++) {
        double d = zeroPart.complexPairs.get(j).sub(p).normSq();
        if (d < bestDist) {
          bestDist = d;
          bestIdx = j;
        }
      }
      cplxZeroForPole[i] = zeroPart.complexPairs.get(bestIdx);
      hasCplxZero[i] = true;
      zeroPart.complexPairs.remove(bestIdx);
    }

    // Largest |zero| first on the list so removeLast() below pops in the
    // same pole-order match.
    zeroPart.realRoots.sort(Comparator.comparingDouble((Double d) -> Math.abs(d)).reversed());

    List<BiquadFilter.Section> out = new ArrayList<>();

    for (int i = 0; i < numCplxPoles; i++) {
      Complex p = polePart.complexPairs.get(i);
      double a1 = -2.0 * p.real();
      double a2 = p.normSq();
      double b0;
      double b1;
      double b2;
      if (hasCplxZero[i]) {
        Complex z = cplxZeroForPole[i];
        b0 = 1.0;
        b1 = -2.0 * z.real();
        b2 = z.normSq();
      } else if (zeroPart.realRoots.size() >= 2) {
        double z1 = zeroPart.realRoots.remove(zeroPart.realRoots.size() - 1);
        double z2 = zeroPart.realRoots.remove(zeroPart.realRoots.size() - 1);
        b0 = 1.0;
        b1 = -(z1 + z2);
        b2 = z1 * z2;
      } else if (!zeroPart.realRoots.isEmpty()) {
        double z = zeroPart.realRoots.remove(zeroPart.realRoots.size() - 1);
        b0 = 1.0;
        b1 = -z;
        b2 = 0.0;
      } else {
        b0 = 1.0;
        b1 = 0.0;
        b2 = 0.0;
      }
      out.add(new BiquadFilter.Section(b0, b1, b2, a1, a2));
    }

    for (double p : polePart.realRoots) {
      double a1 = -p;
      double a2 = 0.0;
      double b0;
      double b1;
      double b2;
      // A real pole takes at most one real zero; leave the rest for any
      // subsequent first-order section.
      if (!zeroPart.realRoots.isEmpty()) {
        double z = zeroPart.realRoots.remove(zeroPart.realRoots.size() - 1);
        b0 = 1.0;
        b1 = -z;
        b2 = 0.0;
      } else {
        b0 = 1.0;
        b1 = 0.0;
        b2 = 0.0;
      }
      out.add(new BiquadFilter.Section(b0, b1, b2, a1, a2));
    }

    if (!out.isEmpty()) {
      BiquadFilter.Section first = out.get(0);
      out.set(
          0,
          new BiquadFilter.Section(
              first.b0 * digital.gain,
              first.b1 * digital.gain,
              first.b2 * digital.gain,
              first.a1,
              first.a2));
    }
    return out;
  }
}
