// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "JacobiElliptic.hpp"

#include <cmath>
#include <complex>
#include <limits>
#include <numbers>
#include <vector>

// All four routines below (EllipticK, Ellipj, InverseJacobiSn, EllipticDegree)
// are tools used by the elliptic filter design path. They follow the
// derivations and equation numbers in:
//
//   Orfanidis, "Introduction to Signal Processing Second Edition (2023)"
//   https://rutgers.app.box.com/s/92is8ajwe2b0liokflkqx1ul2fqqqa7l
//
//   SciPy implementations (for line-by-line comparison):
//   https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py

namespace wpi::math::filter::internal {

namespace {

constexpr int kMaxIter = 60;

// sqrt(1 - k^2) computed as sqrt((1-k)(1+k)) — preserves precision when k is
// small. Real-valued path used by Ellipj/InverseJacobiSn.
double Complement(double k) {
  return std::sqrt((1.0 - k) * (1.0 + k));
}

cplx Complement(cplx k) {
  return std::sqrt((1.0 - k) * (1.0 + k));
}

}  // namespace

double EllipticK(double m) {
  if (m < 0.0 || m > 1.0) {
    return std::numeric_limits<double>::quiet_NaN();
  }
  if (m == 1.0) {
    return std::numeric_limits<double>::infinity();
  }
  // AGM: K(m) = π / (2 · AGM(1, sqrt(1-m))).
  double a = 1.0;
  double b = std::sqrt(1.0 - m);
  for (int i = 0; i < kMaxIter; ++i) {
    if (std::abs(a - b) <= std::numeric_limits<double>::epsilon() * a) {
      break;
    }
    double aNext = 0.5 * (a + b);
    double bNext = std::sqrt(a * b);
    a = aNext;
    b = bNext;
  }
  return std::numbers::pi / (2.0 * a);
}

JacobiResult Ellipj(double u, double m) {
  if (m == 0.0) {
    return {std::sin(u), std::cos(u), 1.0};
  }
  if (m == 1.0) {
    double t = std::tanh(u);
    double sech = 1.0 / std::cosh(u);
    return {t, sech, sech};
  }

  // Ascending Landen: store a_n, c_n at each level until c_n is negligible.
  std::vector<double> a;
  std::vector<double> c;
  a.push_back(1.0);
  double b = std::sqrt(1.0 - m);
  c.push_back(std::sqrt(m));

  int n = 0;
  while (n < kMaxIter) {
    if (std::abs(c.back()) <=
        std::numeric_limits<double>::epsilon() * std::abs(a.back())) {
      break;
    }
    double aN = a.back();
    double bN = b;
    a.push_back(0.5 * (aN + bN));
    b = std::sqrt(aN * bN);
    c.push_back(0.5 * (aN - bN));
    ++n;
  }

  // Descend: phi_n = u · 2^n · a_n, then unwind.
  double phi = u * std::ldexp(a.back(), n);
  for (int j = n; j >= 1; --j) {
    phi = 0.5 * (phi + std::asin((c[j] / a[j]) * std::sin(phi)));
  }
  double sn = std::sin(phi);
  double cn = std::cos(phi);
  // dn = sqrt(1 - m·sn²) — branch chosen so dn ≥ 0 in the principal interval,
  // which matches scipy's convention.
  double dn = std::sqrt(1.0 - m * sn * sn);
  return {sn, cn, dn};
}

cplx InverseJacobiSn(cplx w, double m) {
  // Descending Landen on the modulus: build a sequence of decreasing moduli
  // until the smallest is effectively zero, then invert via arcsin and lift.
  double k = std::sqrt(m);
  if (k > 1.0) {
    return {std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN()};
  }
  if (k == 1.0) {
    // sn(z, 1) = tanh(z), so the inverse is atanh(w).
    return std::atanh(w);
  }

  std::vector<double> ks;
  ks.push_back(k);
  for (int i = 0; i < kMaxIter; ++i) {
    if (ks.back() == 0.0) {
      break;
    }
    double kp = Complement(ks.back());
    double next = (1.0 - kp) / (1.0 + kp);
    ks.push_back(next);
  }

  // Capital K of the original modulus equals (π/2) · ∏(1 + k_i) for i ≥ 1.
  double K = 1.0;
  for (size_t i = 1; i < ks.size(); ++i) {
    K *= (1.0 + ks[i]);
  }
  K *= std::numbers::pi / 2.0;

  std::vector<cplx> wns;
  wns.reserve(ks.size());
  wns.push_back(w);
  for (size_t i = 0; i + 1 < ks.size(); ++i) {
    cplx wn = wns.back();
    cplx wnext =
        (2.0 * wn) / ((1.0 + ks[i + 1]) * (1.0 + Complement(ks[i] * wn)));
    wns.push_back(wnext);
  }

  cplx u = (2.0 / std::numbers::pi) * std::asin(wns.back());
  return K * u;
}

double InverseJacobiSc1(double w, double m) {
  // sc(z, 1-m) = -j · sn(j·z, m), so sc(z, 1-m) = w → sn(j·z, m) = j·w →
  // j·z = arcsn(j·w, m). The result is purely imaginary; return its imag part.
  cplx z = InverseJacobiSn(cplx{0.0, w}, m);
  return z.imag();
}

double EllipticDegree(int N, double m1) {
  // Solve N · K(m)/K'(m) = K(m1)/K'(m1) for m using the q-nome series:
  //   q1 = exp(-π · K'(m1) / K(m1)),  q = q1^(1/N),
  //   m  = 16q · (Σ q^{i(i+1)})⁴ / (1 + 2 Σ q^{i²})⁴
  constexpr int kMmax = 7;
  double K1 = EllipticK(m1);
  double K1p = EllipticK(1.0 - m1);
  double q1 = std::exp(-std::numbers::pi * K1p / K1);
  double q = std::pow(q1, 1.0 / N);

  double num = 0.0;
  for (int i = 0; i <= kMmax; ++i) {
    num += std::pow(q, static_cast<double>(i) * (i + 1));
  }
  double den = 1.0;
  for (int i = 1; i <= kMmax + 1; ++i) {
    den += 2.0 * std::pow(q, static_cast<double>(i) * i);
  }

  double ratio = num / den;
  return 16.0 * q * ratio * ratio * ratio * ratio;
}

}  // namespace wpi::math::filter::internal
