// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "AnalogPrototypes.hpp"

#include <cmath>
#include <complex>
#include <numbers>
#include <stdexcept>
#include <vector>

#include "JacobiElliptic.hpp"

// Each prototype mirrors the corresponding *_ap helper in scipy.signal:
//   buttap, cheb1ap, cheb2ap, ellipap. Those live in
//   https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py
// and are the canonical reference implementations. To verify a coefficient,
// search that file for the function name and compare the closed-form
// pole/zero/gain expressions line-by-line against the body below. Differences
// here are limited to: hand-rolled Complex arithmetic in Java, and
// expm1-based 10^x-1 evaluation for small ripple values (numerically more
// accurate than std::pow(10, x) - 1).
//
// Textbook references for the families themselves:
//   - Butterworth poles on the unit circle:
//     https://en.wikipedia.org/wiki/Butterworth_filter#Transfer_function
//   - Chebyshev I/II pole/zero geometry:
//     https://en.wikipedia.org/wiki/Chebyshev_filter
//   - Elliptic (Cauer): Orfanidis, "Introduction to Signal Processing Second
//   Edition (2023)"
//     https://rutgers.app.box.com/s/92is8ajwe2b0liokflkqx1ul2fqqqa7l

namespace wpi::math::filter::internal {

namespace {

constexpr double kCoefEps = 2e-16;

// 10^x - 1 evaluated as expm1(x · ln10) for accuracy when x is small.
double Pow10m1(double x) {
  constexpr double kLn10 = 2.302585092994045684017991454684;
  return std::expm1(kLn10 * x);
}

}  // namespace

Zpk ButterworthPrototype(int order) {
  // Order-N Butterworth analog low-pass prototype (cutoff 1 rad/s). Poles are
  // the LHP half of the unit circle, evenly spaced at:
  //   p_k = exp( j · (π/2 + π·(2k+1)/(2N)) ),   k = 0..N-1
  // No finite zeros; gain = 1. Matches scipy.signal.buttap.
  // Reference:
  // https://en.wikipedia.org/wiki/Butterworth_filter#Transfer_function
  Zpk p;
  p.gain = 1.0;
  for (int k = 0; k < order; ++k) {
    double angle = std::numbers::pi / 2.0 +
                   std::numbers::pi * (2.0 * k + 1.0) / (2.0 * order);
    p.poles.push_back(std::polar(1.0, angle));
  }
  return p;
}

Zpk ChebyshevIPrototype(int order, double rippleDb) {
  // Order-N Chebyshev type-I analog low-pass prototype (cutoff 1 rad/s).
  // Equiripple in the passband. Matches scipy.signal.cheb1ap exactly.
  // Reference:
  // https://en.wikipedia.org/wiki/Chebyshev_filter#Type_I_Chebyshev_filters_(Chebyshev_filters)
  // SciPy implementation:
  //   https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py
  //   (function cheb1ap)
  //
  // Poles lie on an ellipse in the LHP at:
  //     p_k = -sinh(mu + j*theta_k)
  // where mu = (1/N) * asinh(1/eps), eps = sqrt(10^(rp/10) - 1), and
  // theta_k = pi*(2k - N + 1) / (2N) for k = 0..N-1.
  Zpk out;
  const double eps = std::sqrt(std::pow(10.0, 0.1 * rippleDb) - 1.0);
  const double mu = std::asinh(1.0 / eps) / order;

  cplx prodNegP{1.0, 0.0};
  for (int k = 0; k < order; ++k) {
    const double m = static_cast<double>(-order + 1 + 2 * k);
    const double theta = std::numbers::pi * m / (2.0 * order);
    const cplx pole = -std::sinh(cplx{mu, theta});
    out.poles.push_back(pole);
    prodNegP *= -pole;
  }

  // Gain: forces |H(j0)| = 1 for odd N, 1/sqrt(1+eps^2) for even N (the
  // ripple trough at DC).
  double k = prodNegP.real();
  if (order % 2 == 0) {
    k /= std::sqrt(1.0 + eps * eps);
  }
  out.gain = k;
  return out;
}

Zpk ChebyshevIIPrototype(int order, double stopAttenDb) {
  // Order-N Chebyshev type-II ("inverse Chebyshev") analog low-pass prototype
  // (stopband edge normalized to 1 rad/s — the point at which the response
  // first reaches the stopband attenuation). Equiripple in the stopband.
  // Matches scipy.signal.cheb2ap exactly.
  // Reference:
  // https://en.wikipedia.org/wiki/Chebyshev_filter#Type_II_Chebyshev_filters_(inverse_Chebyshev_filters)
  // SciPy implementation:
  //   https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py
  //   (function cheb2ap)
  //
  // Poles are reciprocals of the deformed unit-circle points; zeros sit on the
  // imaginary axis at j/sin(theta_k).
  Zpk out;
  const double delta = 1.0 / std::sqrt(std::pow(10.0, 0.1 * stopAttenDb) - 1.0);
  const double mu = std::asinh(1.0 / delta) / order;

  cplx prodNegP{1.0, 0.0};
  for (int k = 0; k < order; ++k) {
    const double m1 = static_cast<double>(-order + 1 + 2 * k);
    const double theta = std::numbers::pi * m1 / (2.0 * order);
    const cplx pole = -1.0 / std::sinh(cplx{mu, theta});
    out.poles.push_back(pole);
    prodNegP *= -pole;
  }

  // Zeros at z_k = j / sin(theta_k). For odd order the m=0 entry would give
  // sin(0) → infinite zero; we skip it (one fewer zero than poles, matching
  // scipy's pole-pair conventions for odd-order Chebyshev II).
  cplx prodNegZ{1.0, 0.0};
  for (int k = 0; k < order; ++k) {
    const double m = static_cast<double>(-order + 1 + 2 * k);
    if (m == 0.0) {
      continue;
    }
    const cplx zero{0.0, 1.0 / std::sin(std::numbers::pi * m / (2.0 * order))};
    out.zeros.push_back(zero);
    prodNegZ *= -zero;
  }

  out.gain = (prodNegP / prodNegZ).real();
  return out;
}

Zpk EllipticPrototype(int order, double rippleDb, double stopAttenDb) {
  // Order-N elliptic (Cauer) analog low-pass prototype (cutoff 1 rad/s).
  // Equiripple in both passband and stopband. Matches scipy.signal.ellipap
  // exactly within ~1e-12 for the orders/ripples we test.
  //
  // Primary reference (used to derive the construction below):
  //   Orfanidis, "Introduction to Signal Processing Second Edition (2023)"
  //   https://rutgers.app.box.com/s/92is8ajwe2b0liokflkqx1ul2fqqqa7l
  // SciPy implementation (verbatim algorithm parity):
  //   https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py
  //   (function ellipap)
  //
  // The design proceeds in three stages:
  //   1. Compute the small modulus m1 = eps^2 / (10^(As/10) - 1) and call
  //      EllipticDegree to find the large modulus m that satisfies the
  //      degree equation N·K(m)/K'(m) = K(m1)/K'(m1) (Orfanidis Eq. 49).
  //   2. Place finite zeros at j/(sqrt(m)·sn(j·K/N, m)) for the appropriate
  //      index set (Orfanidis Eq. 64). Conjugate-mirror them.
  //   3. Place poles using the auxiliary point v0 found by inverting
  //      sc(·, 1-m) at 1/eps (Orfanidis §10, Eq. 67–68).
  Zpk out;

  // Two corner cases mirror scipy.signal.ellipap: orders 0 and 1 collapse to
  // closed forms with no zeros / a single real pole. Higher orders run the
  // full Cauer construction below.
  if (order <= 0) {
    out.gain = std::pow(10.0, -rippleDb / 20.0);
    return out;
  }
  if (order == 1) {
    double p = -std::sqrt(1.0 / Pow10m1(0.1 * rippleDb));
    out.poles.push_back(cplx{p, 0.0});
    out.gain = -p;
    return out;
  }

  const double epsSq = Pow10m1(0.1 * rippleDb);
  const double eps = std::sqrt(epsSq);
  const double ck1Sq = epsSq / Pow10m1(0.1 * stopAttenDb);
  if (ck1Sq <= 0.0) {
    throw std::invalid_argument(
        "Elliptic design: invalid ripple/atten combination (small modulus = "
        "0)");
  }

  const double K1 = EllipticK(ck1Sq);
  const double m = EllipticDegree(order, ck1Sq);
  const double capK = EllipticK(m);
  const double sqrtM = std::sqrt(m);

  // Build the index list: for odd N, j = [0, 2, ..., N-1]; for even N,
  // j = [1, 3, ..., N-1]. Length is ceil(N/2).
  std::vector<int> jIdx;
  for (int j = 1 - (order % 2); j < order; j += 2) {
    jIdx.push_back(j);
  }

  // Cache (sn, cn, dn) at each j·K/N — needed for both zeros and poles.
  std::vector<JacobiResult> jacobi;
  jacobi.reserve(jIdx.size());
  for (int j : jIdx) {
    jacobi.push_back(Ellipj(j * capK / order, m));
  }

  // Zeros: z = j · 1/(sqrt(m)·sn), one per index where sn ≠ 0 (drops the j=0
  // entry for odd N — the reciprocal would be a zero at infinity, which we
  // simply omit). Each finite zero pairs with its complex conjugate.
  std::vector<cplx> zerosUpper;
  for (size_t i = 0; i < jacobi.size(); ++i) {
    double sn = jacobi[i].sn;
    if (std::abs(sn) <= kCoefEps) {
      continue;
    }
    cplx z{0.0, 1.0 / (sqrtM * sn)};
    zerosUpper.push_back(z);
  }
  for (const auto& z : zerosUpper) {
    out.zeros.push_back(z);
  }
  for (const auto& z : zerosUpper) {
    out.zeros.push_back(std::conj(z));
  }

  // Poles use an auxiliary point v0 found by inverting sc(·, 1-m) at 1/eps,
  // then ellipj at v0 with the complementary modulus.
  const double r = InverseJacobiSc1(1.0 / eps, ck1Sq);
  const double v0 = capK * r / (order * K1);
  const JacobiResult sv = Ellipj(v0, 1.0 - m);

  std::vector<cplx> polesUpper;
  for (size_t i = 0; i < jacobi.size(); ++i) {
    const double s = jacobi[i].sn;
    const double c = jacobi[i].cn;
    const double d = jacobi[i].dn;
    cplx num{c * d * sv.sn * sv.cn, s * sv.dn};
    double denom = 1.0 - (d * sv.sn) * (d * sv.sn);
    polesUpper.push_back(-num / denom);
  }

  if (order % 2 != 0) {
    // Odd order: one pole is purely real (from j=0). Append complex
    // conjugates for the others; leave the real one alone.
    for (const auto& p : polesUpper) {
      out.poles.push_back(p);
    }
    for (const auto& p : polesUpper) {
      if (std::abs(p.imag()) > kCoefEps) {
        out.poles.push_back(std::conj(p));
      }
    }
  } else {
    for (const auto& p : polesUpper) {
      out.poles.push_back(p);
    }
    for (const auto& p : polesUpper) {
      out.poles.push_back(std::conj(p));
    }
  }

  // Gain: real(prod(-p) / prod(-z)). Even-order trims by sqrt(1+eps²) so DC
  // sits at the ripple trough, matching scipy's convention.
  cplx prodNegP{1.0, 0.0};
  for (const auto& p : out.poles) {
    prodNegP *= -p;
  }
  cplx prodNegZ{1.0, 0.0};
  for (const auto& z : out.zeros) {
    prodNegZ *= -z;
  }
  double k = (prodNegP / prodNegZ).real();
  if (order % 2 == 0) {
    k /= std::sqrt(1.0 + epsSq);
  }
  out.gain = k;
  return out;
}

}  // namespace wpi::math::filter::internal
