// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Zpk.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>

// The four AnalogLpTo* helpers are the standard frequency-domain spectral
// transformations (Oppenheim & Schafer, "Discrete-Time Signal Processing"
// §7.1.5; Constantinides, "Spectral transformations for digital filters",
// IEE Proc. 117 (1970) 1585–1590). They each correspond to a SciPy helper:
//   AnalogLpToLp  ↔  scipy.signal.lp2lp_zpk
//   AnalogLpToHp  ↔  scipy.signal.lp2hp_zpk
//   AnalogLpToBp  ↔  scipy.signal.lp2bp_zpk
//   AnalogLpToBs  ↔  scipy.signal.lp2bs_zpk
// Source for all four:
//   https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py
//
// ZpkToSos pairs conjugate roots into biquad sections using the same
// "nearest pole/zero" pairing scipy.signal.zpk2sos uses by default. SciPy
// reference (function zpk2sos and the helper _cplxreal):
//   https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py
//
// We diverge from scipy in only one place: section ordering. SciPy can return
// a "minimum phase" ordering, while we always sort by ascending |pole| (least
// aggressive first). The cascade product is identical; only the per-section
// numerical conditioning differs.

namespace wpi::math::filter::internal {

namespace {

// A root is treated as real when |imag| falls below this. The same tolerance
// is used to match conjugates, since after bilinear/LP→BP transforms the real
// and imaginary drift of a true pair is of the same order.
constexpr double kImagTolerance = 1e-10;

// Partition a (conjugate-symmetric) root list into a vector of complex roots
// represented by the upper-half-plane conjugate-pair representative, plus a
// vector of real roots.
struct Partitioned {
  std::vector<cplx> complexPairs;  // one representative per conjugate pair
  std::vector<double> realRoots;
};

Partitioned Partition(const std::vector<cplx>& roots) {
  Partitioned out;
  std::vector<bool> matched(roots.size(), false);
  for (size_t i = 0; i < roots.size(); ++i) {
    if (matched[i]) {
      continue;
    }
    matched[i] = true;
    if (std::abs(roots[i].imag()) < kImagTolerance) {
      out.realRoots.push_back(roots[i].real());
      continue;
    }
    // Prefer the upper-half representative.
    cplx rep = roots[i].imag() > 0 ? roots[i] : std::conj(roots[i]);
    // Find unmatched conjugate in the remaining list. Callers pass
    // conjugate-symmetric inputs; if no partner is found the input violated
    // that invariant (or drifted numerically past kImagTolerance), and the
    // cascade that follows would silently double-count the orphan.
    bool found = false;
    for (size_t j = i + 1; j < roots.size(); ++j) {
      if (matched[j]) {
        continue;
      }
      if (std::abs(roots[j].imag() + roots[i].imag()) < kImagTolerance &&
          std::abs(roots[j].real() - roots[i].real()) < kImagTolerance) {
        matched[j] = true;
        found = true;
        break;
      }
    }
    if (!found) {
      throw std::logic_error("Zpk root list is not conjugate-symmetric");
    }
    out.complexPairs.push_back(rep);
  }
  return out;
}

int RelativeDegree(const Zpk& p) {
  return static_cast<int>(p.poles.size()) - static_cast<int>(p.zeros.size());
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
std::pair<cplx, cplx> BpRoots(cplx rScaled, double wo) {
  cplx disc = std::sqrt(rScaled * rScaled - wo * wo);
  return {rScaled + disc, rScaled - disc};
}

}  // namespace

Zpk AnalogLpToLp(const Zpk& p, double wo) {
  Zpk out;
  out.gain = p.gain;
  for (auto& z : p.zeros) {
    out.zeros.push_back(z * wo);
  }
  for (auto& pole : p.poles) {
    out.poles.push_back(pole * wo);
  }
  out.gain *= std::pow(wo, RelativeDegree(p));
  return out;
}

Zpk AnalogLpToHp(const Zpk& p, double wo) {
  // Mirror: s → wo/s. Finite zeros/poles invert and scale; zeros at infinity
  // become zeros at the origin to balance the pole count.
  Zpk out;
  cplx zProd = 1.0;
  cplx pProd = 1.0;
  for (auto& z : p.zeros) {
    out.zeros.push_back(wo / z);
    zProd *= -z;
  }
  for (auto& pole : p.poles) {
    out.poles.push_back(wo / pole);
    pProd *= -pole;
  }
  int degree = RelativeDegree(p);
  for (int i = 0; i < degree; ++i) {
    out.zeros.emplace_back(0.0, 0.0);
  }
  out.gain = p.gain * (zProd / pProd).real();
  return out;
}

Zpk AnalogLpToBp(const Zpk& p, double wo, double bw) {
  Zpk out;
  for (auto& z : p.zeros) {
    auto [z1, z2] = BpRoots(z * bw * 0.5, wo);
    out.zeros.push_back(z1);
    out.zeros.push_back(z2);
  }
  for (auto& pole : p.poles) {
    auto [p1, p2] = BpRoots(pole * bw * 0.5, wo);
    out.poles.push_back(p1);
    out.poles.push_back(p2);
  }
  int degree = RelativeDegree(p);
  for (int i = 0; i < degree; ++i) {
    out.zeros.emplace_back(0.0, 0.0);
  }
  out.gain = p.gain * std::pow(bw, degree);
  return out;
}

Zpk AnalogLpToBs(const Zpk& p, double wo, double bw) {
  Zpk out;
  cplx zProd = 1.0;
  cplx pProd = 1.0;
  for (auto& z : p.zeros) {
    auto [z1, z2] = BpRoots(bw * 0.5 / z, wo);
    out.zeros.push_back(z1);
    out.zeros.push_back(z2);
    zProd *= -z;
  }
  for (auto& pole : p.poles) {
    auto [p1, p2] = BpRoots(bw * 0.5 / pole, wo);
    out.poles.push_back(p1);
    out.poles.push_back(p2);
    pProd *= -pole;
  }
  int degree = RelativeDegree(p);
  const cplx jwo{0.0, wo};
  for (int i = 0; i < degree; ++i) {
    out.zeros.push_back(jwo);
    out.zeros.push_back(-jwo);
  }
  out.gain = p.gain * (zProd / pProd).real();
  return out;
}

Sections ZpkToSos(const Zpk& digital) {
  // A conjugate pair (p, p̄) factors to (z - p)(z - p̄) = z² - 2·Re(p)·z + |p|²,
  // a real-coefficient quadratic — that's how complex roots become the real
  // (b0,b1,b2) and (1,a1,a2) the runtime needs. Same identity for zero pairs.
  //
  // Below: partition roots into complex pairs + lone reals, sort poles by
  // |pole| (least aggressive first, for numerical conditioning), pair each
  // pole pair with its nearest zero pair (scipy's "nearest" rule), and emit
  // one biquad per pole pair (or per real pole for odd order). Leftover real
  // zeros fill in the remaining biquad numerators.
  auto polePart = Partition(digital.poles);
  auto zeroPart = Partition(digital.zeros);

  // Least-aggressive (smallest |pole|) sections go first, so scipy-style
  // golden values line up and the numerically tightest biquad sits last.
  std::sort(
      polePart.complexPairs.begin(), polePart.complexPairs.end(),
      [](const cplx& a, const cplx& b) { return std::norm(a) < std::norm(b); });

  // Pre-assign complex zeros to complex poles using scipy's 'nearest' pairing:
  // process from worst pole (largest |p|, last in ascending sort) to best,
  // each picking the nearest unused complex zero by Euclidean distance.
  // This is deterministic even when all zeros have equal magnitude (e.g.
  // Chebyshev II LP where every digital zero sits on the unit circle).
  int numCplxPoles = static_cast<int>(polePart.complexPairs.size());
  std::vector<cplx> cplxZeroForPole(numCplxPoles, {0.0, 0.0});
  std::vector<bool> hasCplxZero(numCplxPoles, false);
  for (int i = numCplxPoles - 1; i >= 0 && !zeroPart.complexPairs.empty();
       --i) {
    cplx p = polePart.complexPairs[i];
    auto best = std::min_element(zeroPart.complexPairs.begin(),
                                 zeroPart.complexPairs.end(),
                                 [&p](const cplx& a, const cplx& b) {
                                   return std::norm(a - p) < std::norm(b - p);
                                 });
    cplxZeroForPole[i] = *best;
    hasCplxZero[i] = true;
    zeroPart.complexPairs.erase(best);
  }

  // Largest |zero| first on the stack so pops below match the pole order.
  std::sort(zeroPart.realRoots.begin(), zeroPart.realRoots.end(),
            [](double a, double b) { return std::abs(a) > std::abs(b); });

  auto takeZeroPair = [&](Section& s, int poleIdx) {
    if (hasCplxZero[poleIdx]) {
      cplx z = cplxZeroForPole[poleIdx];
      s.b0 = 1.0;
      s.b1 = -2.0 * z.real();
      s.b2 = std::norm(z);
      return;
    }
    if (zeroPart.realRoots.size() >= 2) {
      double z1 = zeroPart.realRoots.back();
      zeroPart.realRoots.pop_back();
      double z2 = zeroPart.realRoots.back();
      zeroPart.realRoots.pop_back();
      s.b0 = 1.0;
      s.b1 = -(z1 + z2);
      s.b2 = z1 * z2;
      return;
    }
    if (!zeroPart.realRoots.empty()) {
      double z = zeroPart.realRoots.back();
      zeroPart.realRoots.pop_back();
      s.b0 = 1.0;
      s.b1 = -z;
      s.b2 = 0.0;
      return;
    }
    s.b0 = 1.0;
    s.b1 = 0.0;
    s.b2 = 0.0;
  };

  Sections out;
  out.reserve(polePart.complexPairs.size() + polePart.realRoots.size());

  for (int i = 0; i < numCplxPoles; ++i) {
    const cplx& p = polePart.complexPairs[i];
    Section s{};
    s.a1 = -2.0 * p.real();
    s.a2 = std::norm(p);
    takeZeroPair(s, i);
    out.push_back(s);
  }

  for (double p : polePart.realRoots) {
    Section s{};
    s.a1 = -p;
    s.a2 = 0.0;
    // A real pole takes at most one real zero; leave the rest for any
    // subsequent first-order section.
    if (!zeroPart.realRoots.empty()) {
      double z = zeroPart.realRoots.back();
      zeroPart.realRoots.pop_back();
      s.b0 = 1.0;
      s.b1 = -z;
      s.b2 = 0.0;
    } else {
      s.b0 = 1.0;
      s.b1 = 0.0;
      s.b2 = 0.0;
    }
    out.push_back(s);
  }

  if (!out.empty()) {
    out[0].b0 *= digital.gain;
    out[0].b1 *= digital.gain;
    out[0].b2 *= digital.gain;
  }
  return out;
}

}  // namespace wpi::math::filter::internal
