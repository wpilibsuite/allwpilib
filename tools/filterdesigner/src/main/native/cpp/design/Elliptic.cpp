// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <complex>
#include <stdexcept>
#include <vector>

#include "JacobiElliptic.hpp"
#include "Zpk.hpp"
#include "wpi/filterdesigner/design/FilterDesign.hpp"

namespace wpi::filterdesigner {

namespace detail {

namespace {

constexpr double kCoefEps = 2e-16;

// 10^x - 1 evaluated as expm1(x · ln10) for accuracy when x is small.
double Pow10m1(double x) {
  constexpr double kLn10 = 2.302585092994045684017991454684;
  return std::expm1(kLn10 * x);
}

}  // namespace

Zpk EllipticPrototype(int order, double rippleDb, double stopbandAttenDb) {
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
  const double ck1Sq = epsSq / Pow10m1(0.1 * stopbandAttenDb);
  if (ck1Sq <= 0.0) {
    throw std::runtime_error(
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

}  // namespace detail

std::optional<Sections> DesignElliptic(FilterKind kind, int order, double fs,
                                       double f1, double f2, double rippleDb,
                                       double stopbandAttenDb) {
  if (order < 1 || fs <= 0.0 || rippleDb <= 0.0 ||
      stopbandAttenDb <= rippleDb) {
    return std::nullopt;
  }
  const double nyquist = 0.5 * fs;
  if (f1 <= 0.0 || f1 >= nyquist) {
    return std::nullopt;
  }
  if ((kind == FilterKind::BandPass || kind == FilterKind::BandStop) &&
      (f2 <= f1 || f2 >= nyquist)) {
    return std::nullopt;
  }
  return detail::DesignFromAnalogLp(
      detail::EllipticPrototype(order, rippleDb, stopbandAttenDb), kind, fs, f1,
      f2);
}

}  // namespace wpi::filterdesigner
