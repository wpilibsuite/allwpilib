// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.filter.internal;

import java.util.ArrayList;
import java.util.List;

/**
 * Elliptic-integral / Jacobi elliptic function helpers used by the elliptic filter prototype.
 *
 * <p>Ports the C++ {@code wpi::math::filter::internal} versions; algorithms and tolerances match
 * Numerical Recipes / SciPy exactly so the generated coefficients agree with {@code
 * scipy.signal.ellip} to within ~1e-10 (LP) for the same inputs.
 *
 * <p>All four routines below ({@code ellipticK}, {@code ellipj}, {@code inverseJacobiSn}, {@code
 * ellipticDegree}) follow the derivations and equation numbers in:
 *
 * <p>Orfanidis, "Introduction to Signal Processing Second Edition (2023)",
 * https://rutgers.app.box.com/s/92is8ajwe2b0liokflkqx1ul2fqqqa7l
 *
 * <p>Specific equations cited inline below (e.g. "Eq. (49)", "Eq. (56)") refer to that PDF. SciPy
 * algorithm parity is also maintained — {@code scipy.special.ellipk} / {@code scipy.special.ellipj}
 * / private {@code _arc_jac_sn} / {@code _arc_jac_sc1} / {@code _ellipdeg} drive {@code
 * scipy.signal.ellipap} (https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py).
 * Numerical Recipes 3rd ed. §6.12 ("Elliptic Integrals and Jacobian Elliptic Functions") covers the
 * AGM and Landen iterations used here.
 */
final class JacobiElliptic {
  private JacobiElliptic() {}

  private static final int MAX_ITER = 60;

  /** Jacobi elliptic functions evaluated at a single point. */
  static final class JacobiResult {
    final double sn;
    final double cn;
    final double dn;

    JacobiResult(double sn, double cn, double dn) {
      this.sn = sn;
      this.cn = cn;
      this.dn = dn;
    }
  }

  // sqrt(1 - k^2) computed as sqrt((1-k)(1+k)) — preserves precision when k is
  // small. Real-valued path used by ellipj/inverseJacobiSn.
  private static double complement(double k) {
    return Math.sqrt((1.0 - k) * (1.0 + k));
  }

  private static Complex complement(Complex k) {
    Complex one = Complex.ONE;
    return one.sub(k).mul(one.add(k)).sqrt();
  }

  /**
   * Complete elliptic integral of the first kind, K(m), via the arithmetic-geometric mean
   * iteration.
   *
   * @param m Parameter (m = k², where k is the modulus). Domain: [0, 1]. m=0 returns π/2; m=1
   *     returns +∞.
   */
  static double ellipticK(double m) {
    if (m < 0.0 || m > 1.0) {
      return Double.NaN;
    }
    if (m == 1.0) {
      return Double.POSITIVE_INFINITY;
    }
    // AGM: K(m) = π / (2 · AGM(1, sqrt(1-m))).
    double a = 1.0;
    double b = Math.sqrt(1.0 - m);
    for (int i = 0; i < MAX_ITER; i++) {
      if (Math.abs(a - b) <= Math.ulp(1.0) * a) {
        break;
      }
      double aNext = 0.5 * (a + b);
      double bNext = Math.sqrt(a * b);
      a = aNext;
      b = bNext;
    }
    return Math.PI / (2.0 * a);
  }

  /**
   * Jacobi elliptic functions sn(u, m), cn(u, m), dn(u, m) for real u and parameter m ∈ [0, 1].
   * Computed via the descending Landen transformation followed by ascending recovery — the same
   * scheme used by Numerical Recipes and (under the hood) SciPy's special.ellipj.
   */
  static JacobiResult ellipj(double u, double m) {
    if (m == 0.0) {
      return new JacobiResult(Math.sin(u), Math.cos(u), 1.0);
    }
    if (m == 1.0) {
      double t = Math.tanh(u);
      double sech = 1.0 / Math.cosh(u);
      return new JacobiResult(t, sech, sech);
    }

    // Ascending Landen: store a_n, c_n at each level until c_n is negligible.
    List<Double> aSeq = new ArrayList<>();
    List<Double> cSeq = new ArrayList<>();
    aSeq.add(1.0);
    double b = Math.sqrt(1.0 - m);
    cSeq.add(Math.sqrt(m));

    int n = 0;
    while (n < MAX_ITER) {
      if (Math.abs(cSeq.get(cSeq.size() - 1))
          <= Math.ulp(1.0) * Math.abs(aSeq.get(aSeq.size() - 1))) {
        break;
      }
      double aN = aSeq.get(aSeq.size() - 1);
      double bN = b;
      aSeq.add(0.5 * (aN + bN));
      b = Math.sqrt(aN * bN);
      cSeq.add(0.5 * (aN - bN));
      n++;
    }

    // Descend: phi_n = u · 2^n · a_n, then unwind.
    double phi = u * Math.scalb(aSeq.get(aSeq.size() - 1), n);
    for (int j = n; j >= 1; j--) {
      phi = 0.5 * (phi + Math.asin((cSeq.get(j) / aSeq.get(j)) * Math.sin(phi)));
    }
    double sn = Math.sin(phi);
    double cn = Math.cos(phi);
    // dn = sqrt(1 - m·sn²) — branch chosen so dn ≥ 0 in the principal interval,
    // which matches scipy's convention.
    double dn = Math.sqrt(1.0 - m * sn * sn);
    return new JacobiResult(sn, cn, dn);
  }

  /**
   * Inverse Jacobi sn: solves sn(z, m) = w for z, where w may be complex. Used by the elliptic
   * filter design to compute v0.
   *
   * <p>Implements the descending-Landen iteration from Orfanidis, "Lecture Notes on Elliptic Filter
   * Design", Eq. (56).
   */
  static Complex inverseJacobiSn(Complex w, double m) {
    // Descending Landen on the modulus: build a sequence of decreasing moduli
    // until the smallest is effectively zero, then invert via arcsin and lift.
    double k = Math.sqrt(m);
    if (k > 1.0) {
      return new Complex(Double.NaN, Double.NaN);
    }
    if (k == 1.0) {
      // sn(z, 1) = tanh(z), so the inverse is atanh(w).
      return w.atanh();
    }

    List<Double> ks = new ArrayList<>();
    ks.add(k);
    for (int i = 0; i < MAX_ITER; i++) {
      double last = ks.get(ks.size() - 1);
      if (last == 0.0) {
        break;
      }
      double kp = complement(last);
      double next = (1.0 - kp) / (1.0 + kp);
      ks.add(next);
    }

    // Capital K of the original modulus equals (π/2) · ∏(1 + k_i) for i ≥ 1.
    double K = 1.0;
    for (int i = 1; i < ks.size(); i++) {
      K *= 1.0 + ks.get(i);
    }
    K *= Math.PI / 2.0;

    List<Complex> wns = new ArrayList<>();
    wns.add(w);
    for (int i = 0; i + 1 < ks.size(); i++) {
      Complex wn = wns.get(wns.size() - 1);
      Complex denom =
          new Complex(1.0 + ks.get(i + 1), 0).mul(Complex.ONE.add(complement(wn.mul(ks.get(i)))));
      Complex wnext = wn.mul(2.0).div(denom);
      wns.add(wnext);
    }

    Complex u = wns.get(wns.size() - 1).asin().mul(2.0 / Math.PI);
    return u.mul(K);
  }

  /**
   * Real inverse Jacobi sc with complementary modulus: solves sc(z, 1-m) = w for real z. Equivalent
   * to scipy's {@code _arc_jac_sc1(w, m)}.
   */
  static double inverseJacobiSc1(double w, double m) {
    // sc(z, 1-m) = -j · sn(j·z, m), so sc(z, 1-m) = w → sn(j·z, m) = j·w →
    // j·z = arcsn(j·w, m). The result is purely imaginary; return its imag part.
    Complex z = inverseJacobiSn(new Complex(0.0, w), m);
    return z.imag();
  }

  /**
   * Solves the elliptic degree equation.
   *
   * <pre>
   *   N · K(m) / K(1-m) = K(m1) / K(1-m1)
   * </pre>
   *
   * <p>For m given the order N and the small modulus parameter m1. Uses the q-nome series of
   * Orfanidis Eq. (49).
   */
  static double ellipticDegree(int N, double m1) {
    // Solve N · K(m)/K'(m) = K(m1)/K'(m1) for m using the q-nome series:
    //   q1 = exp(-π · K'(m1) / K(m1)),  q = q1^(1/N),
    //   m  = 16q · (Σ q^{i(i+1)})⁴ / (1 + 2 Σ q^{i²})⁴
    final int MMAX = 7;
    double K1 = ellipticK(m1);
    double K1p = ellipticK(1.0 - m1);
    double q1 = Math.exp(-Math.PI * K1p / K1);
    double q = Math.pow(q1, 1.0 / N);

    double num = 0.0;
    for (int i = 0; i <= MMAX; i++) {
      num += Math.pow(q, (double) i * (i + 1));
    }
    double den = 1.0;
    for (int i = 1; i <= MMAX + 1; i++) {
      den += 2.0 * Math.pow(q, (double) i * i);
    }

    double ratio = num / den;
    return 16.0 * q * ratio * ratio * ratio * ratio;
  }
}
