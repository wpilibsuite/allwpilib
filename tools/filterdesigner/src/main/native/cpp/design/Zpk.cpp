// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Zpk.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

namespace wpi::filterdesigner::detail {

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
    assert(found && "Zpk root list is not conjugate-symmetric");
    (void)found;
    out.complexPairs.push_back(rep);
  }
  return out;
}

}  // namespace

Sections ZpkToSos(const Zpk& digital) {
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

}  // namespace wpi::filterdesigner::detail
