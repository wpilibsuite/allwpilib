// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.filter.internal;

import java.util.ArrayList;
import java.util.List;

/**
 * Analog low-pass prototype ZPK constructions for the four classical IIR families. Cutoff is
 * normalized to 1 rad/s in every case.
 *
 * <p>Each prototype mirrors the corresponding {@code *_ap} helper in scipy.signal: {@code buttap},
 * {@code cheb1ap}, {@code cheb2ap}, {@code ellipap}. Those live in
 * https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py and are the canonical
 * reference implementations. To verify a coefficient, search that file for the function name and
 * compare the closed-form pole/zero/gain expressions line-by-line against the body below.
 *
 * <p>References for the families themselves:
 *
 * <ul>
 *   <li>Butterworth poles on the unit circle:
 *       https://en.wikipedia.org/wiki/Butterworth_filter#Transfer_function
 *   <li>Chebyshev I/II pole/zero geometry: https://en.wikipedia.org/wiki/Chebyshev_filter
 *   <li>Elliptic (Cauer): Orfanidis, "Introduction to Signal Processing Second Edition (2023)",
 *       https://rutgers.app.box.com/s/92is8ajwe2b0liokflkqx1ul2fqqqa7l
 * </ul>
 */
final class AnalogPrototypes {
  private AnalogPrototypes() {}

  private static final double COEF_EPS = 2e-16;
  private static final double LN10 = 2.302585092994045684017991454684;

  // 10^x - 1 evaluated as expm1(x · ln10) for accuracy when x is small.
  private static double pow10m1(double x) {
    return Math.expm1(LN10 * x);
  }

  // Java's Math has no asinh; poly-fill via the standard identity. The arguments
  // we pass are in (~1, ~100) for typical filter ripple/atten settings, so the
  // simple form is numerically safe.
  private static double asinh(double x) {
    return Math.log(x + Math.sqrt(x * x + 1.0));
  }

  /** Analog Butterworth low-pass prototype, cutoff 1 rad/s. */
  static Zpk butterworthPrototype(int order) {
    // Order-N Butterworth analog low-pass prototype. Poles are the LHP half
    // of the unit circle, evenly spaced at:
    //   p_k = exp( j · (π/2 + π·(2k+1)/(2N)) ),   k = 0..N-1
    // No finite zeros; gain = 1. Matches scipy.signal.buttap.
    // Reference: https://en.wikipedia.org/wiki/Butterworth_filter#Transfer_function
    Zpk p = new Zpk();
    p.gain = 1.0;
    for (int k = 0; k < order; k++) {
      double angle = Math.PI / 2.0 + Math.PI * (2.0 * k + 1.0) / (2.0 * order);
      p.poles.add(Complex.polar(1.0, angle));
    }
    return p;
  }

  /**
   * Analog Chebyshev type-I low-pass prototype, equiripple in passband, cutoff 1 rad/s (the point
   * at which the response first drops to -ripple dB).
   *
   * @param rippleDb Peak-to-peak passband ripple in dB (must be &gt; 0).
   */
  static Zpk chebyshevIPrototype(int order, double rippleDb) {
    // Order-N Chebyshev type-I analog low-pass prototype (cutoff 1 rad/s).
    // Equiripple in the passband. Matches scipy.signal.cheb1ap exactly.
    // Reference:
    // https://en.wikipedia.org/wiki/Chebyshev_filter#Type_I_Chebyshev_filters_(Chebyshev_filters)
    // SciPy implementation:
    //   https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py (function cheb1ap)
    //
    // Poles lie on an ellipse in the LHP at:
    //     p_k = -sinh(mu + j*theta_k)
    // where mu = (1/N) * asinh(1/eps), eps = sqrt(10^(rp/10) - 1), and
    // theta_k = pi*(2k - N + 1) / (2N) for k = 0..N-1.
    Zpk out = new Zpk();
    final double eps = Math.sqrt(Math.pow(10.0, 0.1 * rippleDb) - 1.0);
    final double mu = asinh(1.0 / eps) / order;

    Complex prodNegP = Complex.ONE;
    for (int k = 0; k < order; k++) {
      double m = -order + 1 + 2 * k;
      double theta = Math.PI * m / (2.0 * order);
      Complex pole = new Complex(mu, theta).sinh().negate();
      out.poles.add(pole);
      prodNegP = prodNegP.mul(pole.negate());
    }

    // Gain: forces |H(j0)| = 1 for odd N, 1/sqrt(1+eps^2) for even N (the
    // ripple trough at DC).
    double k = prodNegP.real();
    if (order % 2 == 0) {
      k /= Math.sqrt(1.0 + eps * eps);
    }
    out.gain = k;
    return out;
  }

  /**
   * Analog Chebyshev type-II low-pass prototype, equiripple in stopband, stopband-edge frequency 1
   * rad/s (the point at which the response first reaches {@code stopAttenDb} of attenuation).
   *
   * @param stopAttenDb Stopband attenuation in dB (must be &gt; 0).
   */
  static Zpk chebyshevIIPrototype(int order, double stopAttenDb) {
    // Order-N Chebyshev type-II ("inverse Chebyshev") analog low-pass
    // prototype (stopband edge normalized to 1 rad/s — the point at which the
    // response first reaches the stopband attenuation). Equiripple in the
    // stopband. Matches scipy.signal.cheb2ap exactly.
    // Reference:
    // https://en.wikipedia.org/wiki/Chebyshev_filter#Type_II_Chebyshev_filters_(inverse_Chebyshev_filters)
    // SciPy implementation:
    //   https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py (function cheb2ap)
    //
    // Poles are reciprocals of the deformed unit-circle points; zeros sit on
    // the imaginary axis at j/sin(theta_k).
    Zpk out = new Zpk();
    final double delta = 1.0 / Math.sqrt(Math.pow(10.0, 0.1 * stopAttenDb) - 1.0);
    final double mu = asinh(1.0 / delta) / order;

    Complex prodNegP = Complex.ONE;
    for (int k = 0; k < order; k++) {
      double m1 = -order + 1 + 2 * k;
      double theta = Math.PI * m1 / (2.0 * order);
      Complex pole = Complex.ONE.div(new Complex(mu, theta).sinh()).negate();
      out.poles.add(pole);
      prodNegP = prodNegP.mul(pole.negate());
    }

    // Zeros at z_k = j / sin(theta_k). For odd order the m=0 entry would give
    // sin(0) → infinite zero; we skip it (one fewer zero than poles, matching
    // scipy's pole-pair conventions for odd-order Chebyshev II).
    Complex prodNegZ = Complex.ONE;
    for (int k = 0; k < order; k++) {
      double m = -order + 1 + 2 * k;
      if (m == 0.0) {
        continue;
      }
      Complex zero = new Complex(0.0, 1.0 / Math.sin(Math.PI * m / (2.0 * order)));
      out.zeros.add(zero);
      prodNegZ = prodNegZ.mul(zero.negate());
    }

    out.gain = prodNegP.div(prodNegZ).real();
    return out;
  }

  /**
   * Analog elliptic (Cauer) low-pass prototype: equiripple in both passband and stopband. Cutoff is
   * normalized to 1 rad/s (the point at which the gain first drops below -rippleDb).
   *
   * @param order Filter order (&gt;= 1).
   * @param rippleDb Peak-to-peak passband ripple in dB (&gt; 0).
   * @param stopAttenDb Stopband attenuation in dB (&gt; {@code rippleDb}).
   */
  static Zpk ellipticPrototype(int order, double rippleDb, double stopAttenDb) {
    // Order-N elliptic (Cauer) analog low-pass prototype (cutoff 1 rad/s).
    // Equiripple in both passband and stopband. Matches scipy.signal.ellipap
    // exactly within ~1e-12 for the orders/ripples we test.
    //
    // Primary reference (used to derive the construction below):
    //   Orfanidis, "Introduction to Signal Processing Second Edition (2023)"
    //   https://rutgers.app.box.com/s/92is8ajwe2b0liokflkqx1ul2fqqqa7l
    // SciPy implementation (verbatim algorithm parity):
    //   https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py (function ellipap)
    //
    // The design proceeds in three stages:
    //   1. Compute the small modulus m1 = eps^2 / (10^(As/10) - 1) and call
    //      ellipticDegree to find the large modulus m that satisfies the
    //      degree equation N·K(m)/K'(m) = K(m1)/K'(m1) (Orfanidis Eq. 49).
    //   2. Place finite zeros at j/(sqrt(m)·sn(j·K/N, m)) for the appropriate
    //      index set (Orfanidis Eq. 64). Conjugate-mirror them.
    //   3. Place poles using the auxiliary point v0 found by inverting
    //      sc(·, 1-m) at 1/eps (Orfanidis §10, Eq. 67–68).
    Zpk out = new Zpk();

    // Two corner cases mirror scipy.signal.ellipap: orders 0 and 1 collapse to
    // closed forms with no zeros / a single real pole. Higher orders run the
    // full Cauer construction below.
    if (order <= 0) {
      out.gain = Math.pow(10.0, -rippleDb / 20.0);
      return out;
    }
    if (order == 1) {
      double p = -Math.sqrt(1.0 / pow10m1(0.1 * rippleDb));
      out.poles.add(new Complex(p, 0.0));
      out.gain = -p;
      return out;
    }

    final double epsSq = pow10m1(0.1 * rippleDb);
    final double eps = Math.sqrt(epsSq);
    final double ck1Sq = epsSq / pow10m1(0.1 * stopAttenDb);
    if (ck1Sq <= 0.0) {
      throw new IllegalArgumentException(
          "Elliptic design: invalid ripple/atten combination (small modulus = 0)");
    }

    final double K1 = JacobiElliptic.ellipticK(ck1Sq);
    final double m = JacobiElliptic.ellipticDegree(order, ck1Sq);
    final double capK = JacobiElliptic.ellipticK(m);
    final double sqrtM = Math.sqrt(m);

    // Build the index list: for odd N, j = [0, 2, ..., N-1]; for even N,
    // j = [1, 3, ..., N-1]. Length is ceil(N/2).
    List<Integer> jIdx = new ArrayList<>();
    for (int j = 1 - (order % 2); j < order; j += 2) {
      jIdx.add(j);
    }

    // Cache (sn, cn, dn) at each j·K/N — needed for both zeros and poles.
    List<JacobiElliptic.JacobiResult> jacobi = new ArrayList<>(jIdx.size());
    for (int j : jIdx) {
      jacobi.add(JacobiElliptic.ellipj(j * capK / order, m));
    }

    // Zeros: z = j · 1/(sqrt(m)·sn), one per index where sn ≠ 0 (drops the j=0
    // entry for odd N — the reciprocal would be a zero at infinity, which we
    // simply omit). Each finite zero pairs with its complex conjugate.
    List<Complex> zerosUpper = new ArrayList<>();
    for (JacobiElliptic.JacobiResult jr : jacobi) {
      if (Math.abs(jr.sn) <= COEF_EPS) {
        continue;
      }
      Complex z = new Complex(0.0, 1.0 / (sqrtM * jr.sn));
      zerosUpper.add(z);
    }
    for (Complex z : zerosUpper) {
      out.zeros.add(z);
    }
    for (Complex z : zerosUpper) {
      out.zeros.add(z.conj());
    }

    // Poles use an auxiliary point v0 found by inverting sc(·, 1-m) at 1/eps,
    // then ellipj at v0 with the complementary modulus.
    final double r = JacobiElliptic.inverseJacobiSc1(1.0 / eps, ck1Sq);
    final double v0 = capK * r / (order * K1);
    final JacobiElliptic.JacobiResult sv = JacobiElliptic.ellipj(v0, 1.0 - m);

    List<Complex> polesUpper = new ArrayList<>();
    for (JacobiElliptic.JacobiResult jr : jacobi) {
      double s = jr.sn;
      double c = jr.cn;
      double d = jr.dn;
      Complex num = new Complex(c * d * sv.sn * sv.cn, s * sv.dn);
      double denom = 1.0 - (d * sv.sn) * (d * sv.sn);
      polesUpper.add(num.div(denom).negate());
    }

    if (order % 2 != 0) {
      // Odd order: one pole is purely real (from j=0). Append complex
      // conjugates for the others; leave the real one alone.
      for (Complex p : polesUpper) {
        out.poles.add(p);
      }
      for (Complex p : polesUpper) {
        if (Math.abs(p.imag()) > COEF_EPS) {
          out.poles.add(p.conj());
        }
      }
    } else {
      for (Complex p : polesUpper) {
        out.poles.add(p);
      }
      for (Complex p : polesUpper) {
        out.poles.add(p.conj());
      }
    }

    // Gain: real(prod(-p) / prod(-z)). Even-order trims by sqrt(1+eps²) so DC
    // sits at the ripple trough, matching scipy's convention.
    Complex prodNegP = Complex.ONE;
    for (Complex p : out.poles) {
      prodNegP = prodNegP.mul(p.negate());
    }
    Complex prodNegZ = Complex.ONE;
    for (Complex z : out.zeros) {
      prodNegZ = prodNegZ.mul(z.negate());
    }
    double k = prodNegP.div(prodNegZ).real();
    if (order % 2 == 0) {
      k /= Math.sqrt(1.0 + epsSq);
    }
    out.gain = k;
    return out;
  }
}
